/*
 * Bookr: document reader for the Sony PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef PSP
#include <pspsysmem.h>
#endif

#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "../bkbookmark.h"

#ifdef MAC
static void* memalign(int t, int s) {
	return malloc(s);
}
#else
#include <malloc.h>
#endif

#include "bkpdf.h"

extern "C" {
#include "fitz.h"
#include "mupdf.h"
}

static const float zoomLevels[] = { 0.1f, 0.15f, 0.2f, 0.25f, 0.3f, 0.35f, 0.4f, 0.45f, 0.5f, 0.6f, 0.7f, 0.8f, 0.90f, 1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f,
	1.6f, 1.7f, 1.8f, 1.9f, 2.0f, 2.25f, 2.5f, 2.75f, 3.0f, 3.5f, 4.0f, 5.0f, 7.5f, 10.0f, 16.0f };

static const float rotateLevels[] = { 0.0f, 90.0f, 180.0f, 270.0f };

// singleton
static unsigned int* bounceBuffer = NULL;
//static unsigned int* backBuffer = NULL;
static fz_pixmap* fullPageBuffer = NULL;
static fz_pixmap* cachePageBuffer = NULL;

struct PDFContext {
	/* current document params */
	//string docTitle;
	pdf_xref *xref;
	pdf_outline *outline;
	pdf_pagetree *pages;

	/* current view params */
	/*float zoom;
	int rotate;
	fz_pixmap *image;*/

	/* current page params */
	int pageno;
	pdf_page *page;
	float zoom;
	int zoomLevel;
	float rotate;
	int rotateLevel;

	fz_renderer *rast;
};

static int pdfInit() {

	if (bounceBuffer == NULL) {
		bounceBuffer = (unsigned int*)memalign(16, 480*272*4);
	}
//	if (backBuffer == NULL) {
//		backBuffer = (unsigned int*)memalign(16, 480*272*4);
//	}
	return 0;
}

static PDFContext* pdfOpen(char *filename) {
	PDFContext* ctx = new PDFContext();
	memset(ctx, 0, sizeof(PDFContext));
	fz_error error;
	fz_obj *obj;

	error = fz_newrenderer(&ctx->rast, pdf_devicergb, 0, GLYPHCACHE_SIZE);
	if (error) {
		printf("err0: %d\n", error);
		delete ctx;
		return 0;
	}

	/*
	* Open PDF and load xref table
	*/
	error = pdf_newxref(&ctx->xref);
	if (error) {
		printf("err1: %d\n", error);
		delete ctx;
		return 0;
	}

	error = pdf_loadxref(ctx->xref, filename);
	if (error) {
		printf("err2: %d\n", error);
		//		delete ctx;
		//return 0;

		error = pdf_repairxref(ctx->xref, filename);
		if (error) {
			printf("err3: %d\n", error);
			delete ctx;
			return 0;
		}
	}

	/*
	* Handle encrypted PDF files
	*/

	error = pdf_decryptxref(ctx->xref);
	if (error) {
		delete ctx;
		return (PDFContext*)-1;
	}

	/*if (ctx->xref->crypt) {
		//printf("err5: %d\n", error);
		printf("err5: %d\n", error);
		delete ctx;
		return 0;
	}*/

	if (ctx->xref->crypt) {
	  if (((int)pdf_setpassword(ctx->xref->crypt, "")) != 1) {
			printf("err5: encrypted file (tried empty password): %d\n", error);
			delete ctx;
			return 0;
			/*
			password = winpassword(app, filename);
			if (!password)
				exit(1);
			error = pdf_setpassword(app->xref->crypt, password);
			if (error)
				pdfapp_warn(app, "Invalid password.");
			*/
		}
	}

	/*
	* Load page tree
	*/
	error = pdf_loadpagetree(&ctx->pages, ctx->xref);
	if (error) {
		delete ctx;
		return 0;
	}

	/*
	* Load meta information
	* TODO: move this into mupdf library
	*/
	obj = fz_dictgets(ctx->xref->trailer, "Root");
	if (!obj) {
		printf("err7: %d\n", error);
		//pdfapp_error(app, fz_throw("syntaxerror: missing Root object"));
		delete ctx;
		return 0;
	}

	error = pdf_loadindirect(&ctx->xref->root, ctx->xref, obj);
	if (error) {
		printf("err8: %d\n", error);
		delete ctx;
		return 0;
	}

	obj = fz_dictgets(ctx->xref->trailer, "Info");
	if (obj) {

		error = pdf_loadindirect(&ctx->xref->info, ctx->xref, obj);

		if (error) {
			printf("err10: %d\n", error);
			delete ctx;
			return 0;
		}
	}

	error = pdf_loadnametrees(ctx->xref);
	// non-critical error, we can live without the outline
	if (error) {
		printf("warn11 - no outline: %d\n", error);
	} else {
		error = pdf_loadoutline(&ctx->outline, ctx->xref);
		if (error) {
			printf("warn12 - no outline: %d\n", error);
		}
	}
	// pdf_debugoutline(ctx->outline,0);
/*
	char* ptitle = filename;
	if (strrchr(app->doctitle, '\\'))
		pt = strrchr(app->doctitle, '\\') + 1;
	if (strrchr(app->doctitle, '/'))
		app->doctitle = strrchr(app->doctitle, '/') + 1;
	if (app->xref->info) {
		obj = fz_dictgets(app->xref->info, "Title");
		if (obj) {
			error = pdf_toutf8(&app->doctitle, obj);
			if (error) {
				delete ctx;
				return 0;
			}
		}
	}
*/
	/*
	* Start at first page
	*/
/*
	app->shrinkwrap = 1;
	if (app->pageno < 1)
		app->pageno = 1;
	if (app->zoom <= 0.0)
		app->zoom = 1.0;
	app->rotate = 0;
	app->panx = 0;
	app->pany = 0;

	pdfapp_showpage(app, 1, 1);
*/

	ctx->pageno = 1; 
	ctx->zoomLevel = 13;
	ctx->zoom = zoomLevels[ctx->zoomLevel];
	ctx->rotate = 0.0f;
	ctx->rotateLevel = 0;
	return ctx;
}

static void pdfClose(PDFContext* ctx);

static fz_matrix pdfViewctm(PDFContext* ctx) {
	fz_matrix ctm;
	ctm = fz_identity();
	ctm = fz_concat(ctm, fz_translate(0, -ctx->page->mediabox.y1));
	ctm = fz_concat(ctm, fz_scale(ctx->zoom, -ctx->zoom));
	ctm = fz_concat(ctm, fz_rotate(ctx->rotate + ctx->page->rotate));
	return ctm;
}

static fz_rect screenMediaBox;
static fz_pixmap* pdfRenderTile(PDFContext* ctx, float x, float y, float w, float h, bool transform = false) {
	fz_error error;
	fz_matrix ctm;
	fz_rect bbox;

	bbox.x0 = x;
	bbox.y0 = y;
	bbox.x1 = x;
	bbox.y1 = y;
	ctm = pdfViewctm(ctx);
	if (transform) {
		bbox.x1 += w;
		bbox.y1 += h;
		bbox = fz_transformaabb(ctm, bbox);
	} else {
		bbox.x0 = x;
		bbox.y0 = y;
		bbox.x1 = bbox.x0 + w;
		bbox.y1 = bbox.y0 + h;
	}
	fz_irect ir = fz_roundrect(bbox);
	/*if (!transform) {
		ir.x1 = ir.x0 + w;
		ir.y1 = ir.y0 + h;
	}*/
	fz_pixmap* pix = NULL;
	error = fz_rendertree(&pix, ctx->rast, ctx->page->tree, ctm, ir, 1);
	if (error) {
		return 0;
	}
	if (BKUser::options.pdfInvertColors) {
		unsigned int* s = (unsigned int*)pix->samples;
		unsigned int n = pix->w * pix->h;
		for (unsigned int i = 0; i < n; ++i) {
			*s = ~(*s);
			++s;
		}
	}
	return pix;
}

static void pdfRenderFullPage(PDFContext* ctx) {

	if (fullPageBuffer != NULL) {
		fz_droppixmap(fullPageBuffer);
		fullPageBuffer = NULL;
	}
	if (cachePageBuffer != NULL){
	  fz_droppixmap(cachePageBuffer);
	  cachePageBuffer = NULL;
	}
	float h = ctx->page->mediabox.y1 - ctx->page->mediabox.y0;
	float w = ctx->page->mediabox.x1 - ctx->page->mediabox.x0;
	
	DEBUGBOOKR("FullPage: before renderTile mem:%d\n",((struct mallinfo)mallinfo()).uordblks);
	fullPageBuffer = pdfRenderTile(ctx,
		ctx->page->mediabox.x0,
		ctx->page->mediabox.y0,
		w, h, true);
	DEBUGBOOKR("FullPage: after renderTile mem:%d\n",((struct mallinfo)mallinfo()).uordblks);
	// precalc color shift
	if (fullPageBuffer){
	  unsigned int* s = (unsigned int*)fullPageBuffer->samples;
	  unsigned int n = fullPageBuffer->w * fullPageBuffer->h;
	  for (unsigned int i = 0; i < n; ++i) {
	    *s >>= 8;
	    ++s;
	  }
	}
}

static void recalcScreenMediaBox(PDFContext* ctx) {
	fz_matrix ctm;
	fz_rect bbox;
	float h = ctx->page->mediabox.y1 - ctx->page->mediabox.y0;
	float w = ctx->page->mediabox.x1 - ctx->page->mediabox.x0;
	float x = ctx->page->mediabox.x0;
	float y = ctx->page->mediabox.y0;
	bbox.x0 = x;
	bbox.y0 = y;
	bbox.x1 = x;
	bbox.y1 = y;
	ctm = pdfViewctm(ctx);
	bbox.x1 += w;
	bbox.y1 += h;
	bbox = fz_transformaabb(ctm, bbox);
	screenMediaBox = bbox;
	//printf("sCMB (%g, %g) - (%g, %g)\n", bbox.x0, bbox.y0, bbox.x1, bbox.y1);
}

// PDF page tree optimization --------------------------------------------------

fz_error  fakeImageTile(fz_image *img, fz_pixmap *tile) {

	for (int y = 0; y < tile->h; y++) {
		for (int x = 0; x < tile->w; x++) {
			//tile->samples[(y * tile->w + x) * tile->n] = 255;
			//int i = 255;//tmp->samples[y * tile->w + x] / bpcfact;
			for (int k = 0; k < tile->n; k++) {
				tile->samples[(y * tile->w + x) * tile->n + k] = 255;
			}
			//i = CLAMP(i, 0, src->indexed->high);
			//for (k = 0; k < src->indexed->base->n; k++) {
				//tile->samples[(y * tile->w + x) * tile->n + k + 1] =
					//src->indexed->lookup[i * src->indexed->base->n + k];
			//}
		}
	}

	return 0;
}

static void optimizeNode(fz_node *node);

static void optimizeImage(fz_imagenode *node) {
	fz_image *image = node->image;
	//if (image->w*image->h > 250000) {
	if (image->w*image->h > 1) {
		node->image->w = 1;
		node->image->h = 1;
		node->image->n = 3;
		node->image->a = 0;
		node->image->loadtile = fakeImageTile;
		//void (*drop)(fz_image*);
	}
}

//static void optimizeMeta(fz_metanode *node) {
//	fz_node *child;
//	for (child = node->super.first; child; child = child->next)
//		optimizeNode(child);
//}

static void optimizeOver(fz_overnode *node) {
	fz_node *child;
	for (child = node->super.first; child; child = child->next)
		optimizeNode(child);
}

static void optimizeMask(fz_masknode *node) {
	fz_node *child;
	for (child = node->super.first; child; child = child->next)
		optimizeNode(child);
}

static void optimizeBlend(fz_blendnode *node) {
	fz_node *child;
	for (child = node->super.first; child; child = child->next)
		optimizeNode(child);
}

static void optimizeTransform(fz_transformnode *node) {
	optimizeNode(node->super.first);
}

static void optimizeSolid(fz_solidnode *node) {
}

static void optimizeLink(fz_linknode *node) {
}

static void optimizePath(fz_pathnode *node) {
}

static void optimizeText(fz_textnode *node) {
}

static void optimizeShade(fz_shadenode *node) {
}

static void optimizeNode(fz_node *node) {
	if (!node) {
		return;
	}

	switch (node->kind) {
		case FZ_NIMAGE: optimizeImage((fz_imagenode*)node); break;
		    //case FZ_NMETA: optimizeMeta((fz_metanode*)node); break;
		case FZ_NOVER: optimizeOver((fz_overnode*)node); break;
		case FZ_NMASK: optimizeMask((fz_masknode*)node); break;
		case FZ_NBLEND: optimizeBlend((fz_blendnode*)node); break;
		case FZ_NTRANSFORM: optimizeTransform((fz_transformnode*)node); break;
		case FZ_NCOLOR: optimizeSolid((fz_solidnode*)node); break;
		case FZ_NPATH: optimizePath((fz_pathnode*)node); break;
		case FZ_NTEXT: optimizeText((fz_textnode*)node); break;
		case FZ_NSHADE: optimizeShade((fz_shadenode*)node); break;
		case FZ_NLINK: optimizeLink((fz_linknode*)node); break;
	}
}


int BKPDF::pdfReopenFile(){
  //if ( ctx && fileName){
    if(ctx->xref->file->file > 0){
      //fclose(ctx->xref->file->file);     
      ctx->xref->file->file = open((char*)fileName.c_str(),O_BINARY | O_RDONLY, 0666);
      ctx->xref->file->dead = 0;
    }
    return ctx->xref->file->file;
    //}
    //return 0;
}


static char lastPageError[1024];
static int pdfLoadPage(PDFContext* ctx) {
	if (fullPageBuffer != NULL) {
		fz_droppixmap(fullPageBuffer);
		fullPageBuffer = NULL;
	}
	if (cachePageBuffer != NULL){
	  fz_droppixmap(cachePageBuffer);
	  cachePageBuffer = NULL;
	}
	strcpy(lastPageError, "No error");

	fz_error error;
	fz_obj *obj;

	if (ctx->page)
		pdf_droppage(ctx->page);
	ctx->page = 0;

	// empty store to save memory
 	if (ctx->xref->store){
	  if (BKUser::options.evictGlyphCacheOnNewPage){
	    // empty glyph cache because xref->store is going to be emptied.
	    pdf_emptystore(ctx->xref->store);
	    if (ctx->rast->cache){
	      evictall(ctx->rast->cache);
	    }
	  }
	  else if(pdf_emptystore2(ctx->xref->store) && ctx->rast->cache){
	    evictall(ctx->rast->cache);
	  }
	}
	bk_pdf_resetbufferssize();

	/*
	pdf_item *item;
	fz_obj *key;
	list<fz_obj *key> keys;
	for (item = ctx->xref->store->root; item; item = item->next) {
		if (item->kind == PDF_KIMAGE)
			keys << key;
	}
	list<fz_obj *key>::iterator it(keys.begin);
	while (it != keys.end()) {
		
		++it;
	}
	*/

#ifndef PSP
#define setmeminfolog(...)
#endif

	obj = pdf_getpageobject(ctx->pages, ctx->pageno - 1);

	DEBUGBOOKR("MEM 1:%d\n",MEMINFO);

#ifdef _DEBUGBOOKR
	setmeminfolog(0);
#endif
	error = pdf_loadpage(&ctx->page, ctx->xref, obj);

	DEBUGBOOKR("MEM 2:%d\n",MEMINFO);

	if (error) {

#ifndef PSP
		printf("errLP1: %d\n", error);
#endif

		if(ctx->xref->file->dead){
		  // load failed because we just returned from sleep mode
		  strcpy(lastPageError, "Cannot load page");
		  if (ctx->page)
		    pdf_droppage(ctx->page);
		  ctx->page = 0;
		  
		  // empty store to save memory
		  if (ctx->xref->store){
		    if (BKUser::options.evictGlyphCacheOnNewPage){
		      pdf_emptystore(ctx->xref->store);
		      if (ctx->rast->cache){
			evictall(ctx->rast->cache);
		      }
		    }
		    else if(pdf_emptystore2(ctx->xref->store) && ctx->rast->cache){
		      evictall(ctx->rast->cache);
		    }
		  }
		  bk_pdf_resetbufferssize();
		  return -1;
		}

		if(!BKUser::options.pdfOptimizeForSmallImages){
		  // try to scale small images with current quality setting first...
		  setenv("BOOKR_IMAGE_SCALE_DENOM_ALWAYS_MAX","1",1);

		  if (BKUser::options.pdfImageQuality){
		    if (ctx->page)
		      pdf_droppage(ctx->page);
		    ctx->page = 0;
		    
		    // empty store to save memory
		    if (ctx->xref->store){
		      if (BKUser::options.evictGlyphCacheOnNewPage){
			pdf_emptystore(ctx->xref->store);
			if (ctx->rast->cache){
			  evictall(ctx->rast->cache);
			}
		      }
		      else if(pdf_emptystore2(ctx->xref->store) && ctx->rast->cache){
			evictall(ctx->rast->cache);
		      }
		    }
		    bk_pdf_resetbufferssize();
		    
		    DEBUGBOOKR("MEM 3:%d\n",MEMINFO);
		    
		    error = pdf_loadpage(&ctx->page, ctx->xref, obj);
		    
		    DEBUGBOOKR("MEM 4:%d\n",MEMINFO);

		  }
		}

		if(error){
		  // let's reduce image quality and try again...
		  int imageQuality = BKUser::options.pdfImageQuality;
		  while (imageQuality>=0 && imageQuality < 3){
		    imageQuality++;
		    switch (imageQuality){
		    case 0:
		      setenv("BOOKR_MAX_IMAGE_SCALE_DENOM","1",1);
		      break;
		    case 1:
		      setenv("BOOKR_MAX_IMAGE_SCALE_DENOM","2",1);
		      break;
		    case 2:
		      setenv("BOOKR_MAX_IMAGE_SCALE_DENOM","4",1);
		      break;
		    case 3:
		    default:
		      setenv("BOOKR_MAX_IMAGE_SCALE_DENOM","8",1);
		      break;
		    }
		    
		    if (ctx->page)
		      pdf_droppage(ctx->page);
		    ctx->page = 0;
		    
		    // empty store to save memory
		    if (ctx->xref->store){
		      if (BKUser::options.evictGlyphCacheOnNewPage){
			pdf_emptystore(ctx->xref->store);
			if (ctx->rast->cache){
			  evictall(ctx->rast->cache);
			}
		      }
		      else if(pdf_emptystore2(ctx->xref->store) && ctx->rast->cache){
			evictall(ctx->rast->cache);
		      }
		    }
		    bk_pdf_resetbufferssize();

		    DEBUGBOOKR("MEM 5 %d:%d\n",imageQuality,MEMINFO);

		    error = pdf_loadpage(&ctx->page, ctx->xref, obj);

		    DEBUGBOOKR("MEM 6 %d:%d\n",imageQuality,MEMINFO);
		    if(!error){
		      break;
		    }
		  }

		  
		  // restore original image quality setting.
		  switch (BKUser::options.pdfImageQuality){
		  case 0:
		    setenv("BOOKR_MAX_IMAGE_SCALE_DENOM","1",1);
		    break;
		  case 1:
		    setenv("BOOKR_MAX_IMAGE_SCALE_DENOM","2",1);
		    break;
		  case 2:
		    setenv("BOOKR_MAX_IMAGE_SCALE_DENOM","4",1);
		    break;
		  case 3:
		  default:
		    setenv("BOOKR_MAX_IMAGE_SCALE_DENOM","8",1);
		    break;
		  }

		}

#ifdef _DEBUGBOOKR
		setmeminfolog(0);
#endif
		if(!BKUser::options.pdfOptimizeForSmallImages)    
		  setenv("BOOKR_IMAGE_SCALE_DENOM_ALWAYS_MAX","0",1);

	}

	if (error){
	  strcpy(lastPageError, "Cannot load page");
	  if (ctx->page)
	    pdf_droppage(ctx->page);
	  ctx->page = 0;
	  
	  // empty store to save memory
	  if (ctx->xref->store){
	    if (BKUser::options.evictGlyphCacheOnNewPage){
	      pdf_emptystore(ctx->xref->store);
	      if (ctx->rast->cache){
		evictall(ctx->rast->cache);
	      }
	    }
	    else if(pdf_emptystore2(ctx->xref->store) && ctx->rast->cache){
	      evictall(ctx->rast->cache);
	    }
	  }
	  bk_pdf_resetbufferssize();
	  return -1;
	}
	

	
#ifndef PSP
	if (getenv("DEBUGTREE")){
	  printf("\n\n debug tree ------------------------------------------------\n");
	  fz_debugtree(ctx->page->tree);
	}
	//optimizeNode(ctx->page->tree->root);

	//printf("\n\n OPT debug tree OPT ------------------------------------------------\n");
	//fz_debugtree(ctx->page->tree);
#endif
	recalcScreenMediaBox(ctx);
	if (BKUser::options.pdfFastScroll && ctx->zoom < 2.01f) {
		pdfRenderFullPage(ctx);
	}
	return 0;
}

BKPDF::BKPDF(string& f) : ctx(0), fileName(f), panX(0), panY(0), neg_panX(-1), neg_panY(-1),loadNewPage(false), pageError(false) {
    resetPanXY = false;
    retryLoadPageWhenError = false;
    ctitle = NULL;
}

static BKPDF* singleton = 0;
//static void bkfree_all();
BKPDF::~BKPDF() {
	if(ctitle){
	  fz_free(ctitle);
	}

	if (ctx != 0) {
		saveLastView();
		pdfClose(ctx);
		delete ctx;
	}
	ctx = 0;
	singleton = 0;

	if (fullPageBuffer != NULL) {
		fz_droppixmap(fullPageBuffer);
		fullPageBuffer = NULL;
	}
	if (cachePageBuffer != NULL){
	  fz_droppixmap(cachePageBuffer);
	  cachePageBuffer = NULL;
	}

//#error reactivate the hash!
//#error idea - list allocs linear, list frees linear, do merge every N allocs
	//free_all();
	pdf_donefontlibs();
}

// PDF memory pooling ----------------------------------------------------------

// static long long alloc_mallocs = 0;
// static long long alloc_malloc_size = 0;
// static long long alloc_frees = 0;
// static long long alloc_reallocs = 0;
// static long long alloc_realloc_size = 0;
// static long long alloc_large_realloc = 0;

// static long long alloc_current = 0;
// typedef map<void*, int> AllocMap;
// typedef AllocMap::iterator AllocMapIt;
// static AllocMap alloc_allocs;

// static void reset_allocs() {
// 	alloc_mallocs = 0;
// 	alloc_malloc_size = 0;
// 	alloc_frees = 0;
// 	alloc_reallocs = 0;
// 	alloc_realloc_size = 0;
// 	alloc_large_realloc = 0;
// 	alloc_current = 0;
// 	alloc_allocs.clear();
// }

// static void print_allocs() {
// #ifndef PSP
// /*
// 	printf("\n-----------------------------\n");
// 	printf("alloc_mallocs = %qd\n", alloc_mallocs);
// 	printf("alloc_malloc_size = %qd\n", alloc_malloc_size); 
// 	printf("alloc_frees = %qd\n", alloc_frees);
// 	printf("alloc_reallocs = %qd\n", alloc_reallocs);
// 	printf("alloc_realloc_size = %qd\n", alloc_realloc_size);
// 	printf("alloc_large_realloc = %qd\n", alloc_large_realloc);
// */
// #endif
// }

// static void* bkmalloc(fz_memorycontext *mem, int n) {
// 	void* buf = NULL;
// 	buf = malloc(n);
// 	//memset(buf, 0, n);
// 	alloc_malloc_size += n;
// 	++alloc_mallocs;
// 	alloc_allocs[buf] = n;
// 	alloc_current += n;
// 	return buf;
// }

// static void *bkrealloc(fz_memorycontext *mem, void *p, int n) {
// 	++alloc_reallocs;
// 	alloc_realloc_size += n;
// 	alloc_large_realloc = alloc_large_realloc < n ? n : alloc_large_realloc;
// 	AllocMapIt it = alloc_allocs.find(p);
// 	if (it != alloc_allocs.end()) {
// 		alloc_current -= (*it).second;
// 		alloc_allocs.erase(it);
// 	}
// 	void* r = realloc(p, n);
// 	alloc_allocs[r] = n;
// 	alloc_current += n;
// 	return r;
// }

// static void bkfree(fz_memorycontext *mem, void *p) {
// 	AllocMapIt it = alloc_allocs.find(p);
// 	if (it != alloc_allocs.end()) {
// 		alloc_current -= (*it).second;
// 		alloc_allocs.erase(it);
// 	}
// 	++alloc_frees;
// 	free(p);
// }

// static void bkfree_all() {
// 	//printf("bkfree_all - %d\n", (int)alloc_current);
// 	AllocMapIt it = alloc_allocs.begin();
// 	if (it != alloc_allocs.end()) {
// 		free((*it).first);
// 		++it;
// 	}
// 	alloc_current = 0;
// 	alloc_allocs.clear();
// }

// static fz_memorycontext bkmem = { bkmalloc, bkrealloc, bkfree };

static bool lastScrollFlag = false;
BKPDF* BKPDF::create(string& file,string& longFileName) {
	if (singleton != 0) {
		printf("cannot open more than 1 pdf at the same time\n");
		return singleton;
	}

	//reset_allocs();
	BKPDF* b = new BKPDF(file);
	singleton = b;
	b->longFileName = longFileName;
	b->xpos_mode = 0;
	//	fz_setmemorycontext(&bkmem);
	fz_cpudetect();
	fz_accelerate();

	pdfInit();
	PDFContext* ctx = pdfOpen((char*)file.c_str());
	if (ctx == 0) {
		delete b;
		return 0;
	}
	if ((int)ctx == -1) {
	  delete b;
	  return (BKPDF*) -1;
	}
	b->ctx = ctx;
	int lastSlash = -1;
	int n = b->longFileName.size();
	for (int i = 0; i < n; ++i) {
		if (b->longFileName[i] == '\\')
			lastSlash = i;
		else if (b->longFileName[i] == '/')
			lastSlash = i;
	}
	b->title.assign(b->longFileName, lastSlash+1, n - 1 - lastSlash);
	b->ctitle = NULL;
	if (ctx->xref->info) {
		fz_error error;
		fz_obj *obj;
		obj = fz_dictgets(ctx->xref->info, "Title");
		if (obj) {
			error = pdf_toutf8(&b->ctitle, obj);
			if (error == NULL) {
				b->title = b->ctitle;
			}
		}
	}
	// Add bookmark support
	//int position = BKBookmark::getLastView(b->filePath);
	//b->setPage(position);
	

	//load from bookmark.xml if available
	if (b->isBookmarkable()){
	    BKBookmark bm;
	    string fn;
	    b->getFileName(fn);
	    if (BKBookmarksManager::getLastView(fn, bm)) {  
	      b->setBookmarkPosition(bm.viewData);
	      b->setZoom(bm.zoom);
	    }
	}

 	b->pageError = pdfLoadPage(ctx) != 0;
 	if (!b->pageError){
 	    b->retryLoadPageWhenError = true;
 	}
       	else if(b->retryLoadPageWhenError){
	  b->pdfReopenFile();
	  b->pageError = pdfLoadPage(ctx) != 0;
	  if (!b->pageError){
 	    b->retryLoadPageWhenError = true;
	  }
	  else{
	    delete b;
	    return NULL;
	  }
	}
// 	else{
// 	  delete b;
// 	  return NULL;
// 	}
	b->redrawBuffer();
	//print_allocs();
 	FZScreen::resetReps();
	lastScrollFlag = BKUser::options.pdfFastScroll;
	b->loadNewPage = false;
	char t[256];
	snprintf(t, 256, "Page %d of %d", ctx->pageno, pdf_getpagecount(ctx->pages));
	b->setBanner(t);
	return b;
}

static void pdfClose(PDFContext* ctx) {
	if (ctx->page)
		pdf_droppage(ctx->page);
	ctx->page = 0;

	if (ctx->pages)
		pdf_droppagetree(ctx->pages);
	ctx->pages = 0;

	/*if (ctx->image)
		fz_droppixmap(ctx->image);
	ctx->image = nil;*/

	if (ctx->outline)
		pdf_dropoutline(ctx->outline);
	ctx->outline = 0;

	if (ctx->xref->store)
		pdf_dropstore(ctx->xref->store);
	ctx->xref->store = 0;

	pdf_closexref(ctx->xref);
	ctx->xref = 0;

	if (ctx->rast)
		fz_droprenderer(ctx->rast);
	ctx->rast = 0;
	
	if(bounceBuffer){
	  free(bounceBuffer);
	  bounceBuffer = NULL;
	}
	//printf("alloc_current = %d\n", (int)alloc_current);
}

void BKPDF::clipCoords(float& nx, float& ny) {
	if (!pageError) {
		//fz_matrix ctm = pdfViewctm(ctx);
		//fz_rect bbox = ctx->page->mediabox;
		fz_rect bbox = screenMediaBox;
		//bbox = fz_transformaabb(ctm, bbox);
		if (ny < 0.0f) {
			ny = 0.0f;
		}
		float h = bbox.y1 - bbox.y0;
		if (h <= 272.0f) {
			ny = 0.0f;
		} else if (ny > h - 272.0f) {
			ny = h - 272.0f;
		}
		if (nx < 0.0f) {
			nx = 0.0f;
		}
		float w = bbox.x1 - bbox.x0;
		if (w <= 480.0f) {
			nx = 0.0f;
		} else if (nx > w - 480.0f) {
			nx = w - 480.0f;
		}
		//printf("cc panX, panY - %g, %g\n", nx, ny);
	}
}

bool BKPDF::isZoomable() {
	return true;
}

void BKPDF::getZoomLevels(vector<BKDocument::ZoomLevel>& v) {
  //int n = BKUser::options.pdfFastScroll ? 15 : sizeof(zoomLevels)/sizeof(float);
	for (int i = 0; i < sizeof(zoomLevels)/sizeof(float); ++i){
	  if(BKUser::options.pdfFastScroll && zoomLevels[i]>2.01)
	    break;
	  v.push_back(BKDocument::ZoomLevel(BKDOCUMENT_ZOOMTYPE_ABSOLUTE, "FIX ZOOM LABELS"));
	}
}

int BKPDF::getCurrentZoomLevel() {
	int zl = ctx->zoomLevel;
// 	if(zoomLevels[zl] != ctx->zoom){
// 	  zl = 0;
// 	  while (zoomLevels[zl]<ctx->zoom){
// 	    zl++;
// 	  }
// 	}
	return zl;
}

int BKPDF::setZoomLevel2(int z){
	if (z == ctx->zoomLevel)
		return 0;
	int n = sizeof(zoomLevels)/sizeof(float);
	ctx->zoomLevel = z;

	if (ctx->zoomLevel < 0)
		ctx->zoomLevel = 0;
	if (ctx->zoomLevel >= n)
		ctx->zoomLevel = n - 1;
  	ctx->zoom = zoomLevels[ctx->zoomLevel];
	return ctx->zoomLevel;
} 

int BKPDF::setZoomLevel(int z) {
	if (z == ctx->zoomLevel)
		return 0;
	
	if (z == ctx->zoomLevel+1 && zoomLevels[ctx->zoomLevel] > ctx->zoom)
	  z -= 1;

	int n = sizeof(zoomLevels)/sizeof(float);
	ctx->zoomLevel = z;

	if (ctx->zoomLevel < 0){
		ctx->zoomLevel = 0;
	}
	if (ctx->zoomLevel >= n){
		ctx->zoomLevel = n - 1;
	}
	if (BKUser::options.pdfFastScroll && zoomLevels[ctx->zoomLevel] > 2.01f) {
		int i;
		for(i=0;i<n;i++){
		  if(zoomLevels[i]>2.01f) 
		    break;
		}
		ctx->zoomLevel = i-1;
		//ctx->zoom = zoomLevels[ctx->zoomLevel];
	}

	if (ctx->zoom == zoomLevels[ctx->zoomLevel]){
	  char t[256];
	  snprintf(t, 256, "Zoom %2.3gx", ctx->zoom);
	  setBanner(t);
	  //	  redrawBuffer();
	  return 0;
	}

	panX = int(float( panX + 240 ) * zoomLevels[ctx->zoomLevel] / ctx->zoom - 240);
	panY = int(float( panY + 136 ) * zoomLevels[ctx->zoomLevel] / ctx->zoom - 136);

	if(xpos_mode || alwaysSetXPos){
	  int* currentPage;
	  int* adjPage;
	  if (ctx->pageno%2){ // current page is odd page
	    currentPage = &xpos_odd;
	    adjPage = &xpos_even;
	  }
	  else{ // current page is even page.
	    currentPage = &xpos_even;
	    adjPage = &xpos_odd;
	  }
	
	  if(ctx->rotateLevel%2){ //vetical mode, take panY value
	    *currentPage = panY;
	    *adjPage = int(float( *adjPage + 136 ) * zoomLevels[ctx->zoomLevel] / ctx->zoom - 136);
	  }
	  else{ // normal or upsidedown mode, take panX value
	    *currentPage = panX;
	    *adjPage = int(float( *adjPage + 240 ) * zoomLevels[ctx->zoomLevel] / ctx->zoom - 240);
	  }
	}

	ctx->zoom = zoomLevels[ctx->zoomLevel];

// 	float nx = float(panX)*ctx->zoom;
// 	float ny = float(panY)*ctx->zoom;
// 	clipCoords(nx, ny);
// 	panX = int(nx);
// 	panY = int(ny);

	char t[256];
	snprintf(t, 256, "Zoom %2.3gx", ctx->zoom);
	setBanner(t);
	if (BKUser::options.pageScrollCacheMode) {
// 		pdfRenderFullPage(ctx);
		loadNewPage = true;
		resetPanXY = false;
		return BK_CMD_MARK_DIRTY;
	}

	redrawBuffer(true);
	return BK_CMD_MARK_DIRTY;
}


bool BKPDF::hasZoomToFit() {
	return true;
}

int BKPDF::setZoomToFitWidth() {
	float nw = fabs(screenMediaBox.x1 - screenMediaBox.x0)/ctx->zoom;		// width at 1.0 zoom
	if (nw == 0.0f)
		nw = 1.0f;
	ctx->zoom = 480.0f/nw;

	int newzl = ctx->zoomLevel;
	if(zoomLevels[newzl] != ctx->zoom){
	  newzl = 0;
	  while (zoomLevels[newzl]<ctx->zoom){
	    newzl++;
	  }
	  ctx->zoomLevel = newzl;
	}

// 	float nx = float(panX)*ctx->zoom;
// 	float ny = float(panY)*ctx->zoom;
// 	clipCoords(nx, ny);
// 	panX = int(nx);
// 	panY = int(ny);
	
	char t[256];
	snprintf(t, 256, "Zoom %2.3gx", ctx->zoom);
	setBanner(t);
	if (BKUser::options.pageScrollCacheMode) {
// 		pdfRenderFullPage(ctx);
		loadNewPage = true;
		resetPanXY = false;
		return BK_CMD_MARK_DIRTY;
	}
	redrawBuffer(true);
	return BK_CMD_MARK_DIRTY;
}

int BKPDF::setZoomToFitHeight() {
	float nh = fabs(screenMediaBox.y1 - screenMediaBox.y0)/ctx->zoom;		// height at 1.0 zoom
	if (nh == 0.0f)
		nh = 1.0f;
	ctx->zoom = 272.0f/nh;

	int newzl = ctx->zoomLevel;
	if(zoomLevels[newzl] != ctx->zoom){
	  newzl = 0;
	  while (zoomLevels[newzl]<ctx->zoom){
	    newzl++;
	  }
	  ctx->zoomLevel = newzl;
	}

/*
	float nx = float(panX)*ctx->zoom;
	float ny = float(panY)*ctx->zoom;
	clipCoords(nx, ny);
	panX = int(nx);
	panY = int(ny);
*/
	char t[256];
	snprintf(t, 256, "Zoom %2.3gx", ctx->zoom);
	setBanner(t);
	if (BKUser::options.pageScrollCacheMode) {
// 		pdfRenderFullPage(ctx);
		loadNewPage = true;
		resetPanXY = false;
		return BK_CMD_MARK_DIRTY;
	}
	redrawBuffer(true);
	return BK_CMD_MARK_DIRTY;
}

int BKPDF::setZoomIn(int left, int right) {
	
  // calc new zoom, 16x max.
	float oldZoom = ctx->zoom;
	ctx->zoom = oldZoom * 480.0f/(right-left);
	if (ctx->zoom > 16.0f)
	  ctx->zoom = 16.0f;
	    
	int newzl = ctx->zoomLevel;
	if(zoomLevels[newzl] != ctx->zoom){
	  newzl = 0;
	  while (zoomLevels[newzl]<ctx->zoom){
	    newzl++;
	  }
	  ctx->zoomLevel = newzl;
	}

	float nx,ny;
	nx = ((float)panX  + left - leftMargin ) * ctx->zoom/oldZoom;
	ny = float( panY ) * ctx->zoom / oldZoom;
	
	panX = int(nx);
	panY = int(ny);

	if(xpos_mode || alwaysSetXPos){
	  int* currentPage;
	  int* adjPage;
	  if (ctx->pageno%2){ // current page is odd page
	    currentPage = &xpos_odd;
	    adjPage = &xpos_even;
	  }
	  else{ // current page is even page.
	    currentPage = &xpos_even;
	    adjPage = &xpos_odd;
	  }
	
	  if(ctx->rotateLevel%2){ //vetical mode, take panY value
	    *currentPage = panY;
	    *adjPage = int(float( *adjPage ) * ctx->zoom / oldZoom);
	  }
	  else{ // normal or upsidedown mode, take panX value
	    *currentPage = panX;
	    *adjPage = int(((float)(*adjPage)  + left - leftMargin ) * ctx->zoom/oldZoom);
	  }
	}

	char t[256];
	snprintf(t, 256, "Zoom %2.3gx", ctx->zoom);
	setBanner(t);
	if (BKUser::options.pageScrollCacheMode) {
// 		pdfRenderFullPage(ctx);
		loadNewPage = true;
		resetPanXY = false;
		return BK_CMD_MARK_DIRTY;
	}
	resetPanXY = false;
	redrawBuffer(true);
	return BK_CMD_MARK_DIRTY;
}

bool BKPDF::isBookmarkable() {
	return true;
}

void BKPDF::getBookmarkPosition(map<string, int>& m) {
	m["page"] = ctx->pageno;
	m["zoom"] = ctx->zoomLevel;
	m["panX"] = panX;
	m["panY"] = panY;
	m["rotation"] = ctx->rotateLevel;
	m["xpos_mode"] = xpos_mode;
	m["xpos_even"] = xpos_even;
	m["xpos_odd"] = xpos_odd;
}

int BKPDF::setBookmarkPosition(map<string, int>& m) {
	setCurrentPage(m["page"]);
	setZoomLevel2(m["zoom"]);
	setRotation2(m["rotation"]);
	panX = m["panX"];
	panY = m["panY"];
	xpos_mode = m["xpos_mode"];
	xpos_even = m["xpos_even"];
	xpos_odd = m["xpos_odd"];
	loadNewPage = true;
	resetPanXY = false;
	return BK_CMD_MARK_DIRTY;
}

bool BKPDF::isPaginated() {
	return true;
}

int BKPDF::getTotalPages() {
	return pdf_getpagecount(ctx->pages);
}

int BKPDF::getCurrentPage() {
	return ctx->pageno;
}

int BKPDF::setCurrentPage(int position) {
	int oldPage = ctx->pageno; 
	ctx->pageno = position;
	if (ctx->pageno < 1)
		ctx->pageno = 1;
	if (ctx->pageno > pdf_getpagecount(ctx->pages))
		ctx->pageno = pdf_getpagecount(ctx->pages);
	if (ctx->pageno != oldPage) {
		loadNewPage = true;
		resetPanXY = true;
		char t[256];
		snprintf(t, 256, "Loading page %d", ctx->pageno);
		setBanner(t);
		//panY = 0;
		return BK_CMD_MARK_DIRTY;
	}
	return 0;
}

bool BKPDF::isRotable() {
	return true;
}

int BKPDF::getRotation() {
	return ctx->rotateLevel;
}
int BKPDF::setRotation2(int z, bool bForce) {

	int n = 4;
	ctx->rotateLevel = z;

	if (ctx->rotateLevel < 0)
		ctx->rotateLevel = 3;
	if (ctx->rotateLevel >= n)
		ctx->rotateLevel = 0;

	ctx->rotate = rotateLevels[ctx->rotateLevel];
	return ctx->rotate;
}

int BKPDF::setRotation(int z, bool bForce) {
	if (z == ctx->rotateLevel)
		return 0;

	int panYc = panY + 136;
	int panXc = panX + 240;

	switch (z - ctx->rotateLevel) {
	    case 1:  // clockwise
		panY = panXc - 136;
		panX = int(fabs(screenMediaBox.y1 - screenMediaBox.y0)) - panYc - 240;

		if(xpos_mode || alwaysSetXPos){
		  int* currentPage;
		  int* adjPage;
		  if (ctx->pageno%2){ // current page is odd page
		    currentPage = &xpos_odd;
		    adjPage = &xpos_even;
		  }
		  else{ // current page is even page.
		    currentPage = &xpos_even;
		    adjPage = &xpos_odd;
		  }
		  
		  if(z%2){ //vetical mode, take panY value
		    *currentPage = panY;
		    *adjPage = *adjPage+240-136;
		  }
		  else{ // normal or upsidedown mode, take panX value
		    *currentPage = panX;
		    *adjPage = int(fabs(screenMediaBox.y1 - screenMediaBox.y0)) - (*adjPage + 136) - 240;
		  }
		}

		break;
	    case -1:  // counter clockwise
		panX = panYc - 240;
		panY = int(fabs(screenMediaBox.x1 - screenMediaBox.x0)) - panXc - 136;

		if(xpos_mode || alwaysSetXPos){
		  int* currentPage;
		  int* adjPage;
		  if (ctx->pageno%2){ // current page is odd page
		    currentPage = &xpos_odd;
		    adjPage = &xpos_even;
		  }
		  else{ // current page is even page.
		    currentPage = &xpos_even;
		    adjPage = &xpos_odd;
		  }
		  
		  if(z%2){ //vetical mode, take panY value
		    *currentPage = panY;
		    *adjPage = int(fabs(screenMediaBox.x1 - screenMediaBox.x0)) - (*adjPage+240) - 136;
		  }
		  else{ // normal or upsidedown mode, take panX value
		    *currentPage = panX;
		    *adjPage = (*adjPage + 136) - 240;
		  }
		}

		break;
	    default:  //error, shouldn't be here.
		;
	}

	int n = 4;
	ctx->rotateLevel = z;

	if (ctx->rotateLevel < 0)
		ctx->rotateLevel = 3;
	if (ctx->rotateLevel >= n)
		ctx->rotateLevel = 0;

	ctx->rotate = rotateLevels[ctx->rotateLevel];

/*	
	float nx = float(panX);
	float ny = float(panY);
	clipCoords(nx, ny);
	panX = int(nx);
	panY = int(ny);
*/
	char t[256];
	snprintf(t, 256, "Rotate to %3.3g°", ctx->rotate);
	setBanner(t);

	if (BKUser::options.pageScrollCacheMode) {
// 		pdfRenderFullPage(ctx);
		loadNewPage = true;
		resetPanXY = false;
		return BK_CMD_MARK_DIRTY;
	}
	
	redrawBuffer();
	return BK_CMD_MARK_DIRTY;
}

void BKPDF::getTitle(string& s, int type) {
  switch(type){
  case 1:
    s = ctitle?ctitle:"<No Title Info>";
    break;
  case 2:
    s = longFileName;
    break;
  case 3:
    s = ctitle?ctitle:"<No Title Info>";
    s += " ["; 
    s += longFileName;
    s += "]";
    break;
  case 4:
    s = longFileName;
    s += " [";
    s += ctitle?ctitle:"<No Title Info>";
    s += "]";
    break;
  default:
    s = title;
    break;
  }

}

void BKPDF::getType(string& s) {
	s = "PDF";
}

void BKPDF::getFileName(string& s) {
	s = fileName;
}

int BKPDF::prePan(int x, int y) {
	float nx = float(panX + x);
	float ny = float(panY + y);
	clipCoords(nx, ny);
	if (panX == int(nx) && panY == int(ny))
		return 0;
	panBuffer(int(nx), int(ny));
	return BK_CMD_MARK_DIRTY;
}

int BKPDF::pan(int x, int y) {
  
  int minValueX = 32 * BKUser::options.analogRateX / 100;
  int minValueY = 32 * BKUser::options.analogRateY / 100;
	if (x > -1 * minValueX && x < minValueX)
		x = 0;
	if (y > -1 * minValueY && y < minValueY)
		y = 0;
	if (x == 0 && y == 0)
		return 0;
	x >>= 2;
	y >>= 2;
	return prePan(x, y);
}

int BKPDF::screenUp() {
    return prePan(0, -1 * BKUser::options.vScroll);
}

int BKPDF::screenDown() {
    return prePan(0, BKUser::options.vScroll);
}

int BKPDF::screenLeft() {
    return prePan(-1 * BKUser::options.hScroll, 0);
}

int BKPDF::screenRight() {
    return prePan(BKUser::options.hScroll, 0);
}

void BKPDF::renderContent() {
	// the copyImage fills the entire screen
	//FZScreen::clear(0xffffff, FZ_COLOR_BUFFER);
	FZScreen::color(0xffffffff);
	FZScreen::matricesFor2D();
	FZScreen::enable(FZ_TEXTURE_2D);
	FZScreen::enable(FZ_BLEND);
	FZScreen::blendFunc(FZ_ADD, FZ_SRC_ALPHA, FZ_ONE_MINUS_SRC_ALPHA);

	FZScreen::copyImage(FZ_PSM_8888, 0, 0, 480, 272, 480, bounceBuffer, 0, 0, 512, (void*)(0x04000000+(unsigned int)FZScreen::framebuffer()));

	if (pageError) {
		texUI->bindForDisplay();
		FZScreen::ambientColor(0xf06060ff);
		drawRect(0, 126, 480, 26, 6, 31, 1);
		fontBig->bindForDisplay();
		FZScreen::ambientColor(0xff222222);
		char t[256];
		snprintf(t, 256, "Error in page %d: %s", ctx->pageno, lastPageError);
		drawTextHC(t, fontBig, 130);
	}

#ifdef PSP
	//int tfm = sceKernelTotalFreeMemSize();
	//int mfm = sceKernelMaxFreeMemSize();
	//printf("tfm %d, mfm %d\n", tfm, mfm);
#endif
}

static inline void bk_memcpysr8(void* to, void* from, unsigned int n) {
	int* s = (int*)from;
	int* d = (int*)to;
	n >>= 2;
	for (unsigned int i = 0; i < n; ++i) {
		*d = *s >> 8;
		++s;
		++d;
	}
}

#ifdef PSP
extern "C" {
	extern int bk_memcpy(void*, void*, unsigned int);
	// not working...
	//extern int bk_memcpysr8(void*, void*, unsigned int);
};
#else
#define bk_memcpy memcpy
#endif

bool BKPDF::redrawBuffer(bool setSpeed) {
	if (pageError)
		return false;
	
	if(setSpeed)
	  FZScreen::setSpeed(BKUser::options.pspMenuSpeed);
	recalcScreenMediaBox(ctx);

	if(neg_panX>=0){
	  panX = fabs(screenMediaBox.x1 - screenMediaBox.x0) - neg_panX;
	  neg_panX = -1;
	}
	if(neg_panY>=0){
	  panY = fabs(screenMediaBox.y1 - screenMediaBox.y0) - neg_panY;
	  neg_panY = -1;
	}
	
	if (loadNewPage && resetPanXY) {

		if(xpos_mode){
		  int* currentPage;
		  if (ctx->pageno%2){ // current page is odd page
		    currentPage = &xpos_odd;
		  }
		  else{ // current page is even page.
		    currentPage = &xpos_even;
		  }
		  
		  if(ctx->rotateLevel%2){ //vetical mode, set panY value
		    panY = *currentPage;
		  }
		  else{ // normal or upsidedown mode, set panX value
		    panX = *currentPage;
		  }
		}


		// set panX or panY according to rotation
		switch (ctx->rotateLevel){
		    case 0: //up
			panY = 0;
			break;
		    case 1: //right
			panX =  int(fabs(screenMediaBox.x1 - screenMediaBox.x0)) - 480;
			if (panX<0) panX = 0;
			break;
		    case 2: //down
			panY =  int(fabs(screenMediaBox.y1 - screenMediaBox.y0)) - 272;
			if (panY<0) panY = 0;
			break;
		    case 3: //left
			panX = 0;
			break;
		}
	}


	if (BKUser::options.pdfFastScroll && fullPageBuffer != NULL) {
		// copy region of the full page buffer
		int cw = 480;
		bool fillGrey = false;
		int dskip = 0;
		//int px = panX;
		//int py = panY;
		float nx = float(panX);
		float ny = float(panY);
		clipCoords(nx, ny);
		int px = int(nx);
		int py = int(ny);

		//printf("panX, panY - %d, %d\n", px, py);
		leftMargin = 0;
		if (fullPageBuffer->w < 480) {
			px = 0;
			cw = fullPageBuffer->w;
			fillGrey = true;
			dskip += (480 - fullPageBuffer->w) / 2;
			leftMargin = (480 - fullPageBuffer->w) / 2;
		} else if (px + 480 > fullPageBuffer->w) {
			px = fullPageBuffer->w - 480;
		}
		int ch = 272;
		if (fullPageBuffer->h < 272) {
			py = 0;
			ch = fullPageBuffer->h;
			fillGrey = true;
			dskip += ((272 - fullPageBuffer->h) / 2)*480;
		} else if (py + 272 > fullPageBuffer->h) {
			py = fullPageBuffer->h - 272;
		}
		//printf("px, py - %d, %d\n", px, py);
		panX = px;
		panY = py;

		if(xpos_mode || alwaysSetXPos){
		  int* currentPage;
		  if (ctx->pageno%2){ // current page is odd page
		    currentPage = &xpos_odd;
		  }
		  else{ // current page is even page.
		    currentPage = &xpos_even;
		  }
		  
		  if(ctx->rotateLevel%2){ //vetical mode, take panY value
		    *currentPage = panY;
		  }
		  else{ // normal or upsidedown mode, take panX value
		    *currentPage = panX;
		  }
		}

		unsigned int* s = (unsigned int*)fullPageBuffer->samples + px + (fullPageBuffer->w*py);
		unsigned int* d = bounceBuffer;
		if (fillGrey) {
			unsigned int *dd = d;
			const unsigned int c = BKUser::options.colorSchemes[BKUser::options.currentScheme].txtBGColor;
			for (int i = 0; i < 480*272; i++) {
				*dd = c;
				++dd;
			}
		}
		d += dskip;
		for (int j = 0; j < ch; ++j) {
			bk_memcpy(d, s, cw*4);
			d += 480;
			s += fullPageBuffer->w;
		}
		setThumbnail(fullPageBuffer->w, fullPageBuffer->h, 480, 272, px, py);
		if(setSpeed)
		  FZScreen::setSpeed(BKUser::options.pspSpeed);

		return true;
	}
	/*
	fz_pixmap* pix = pdfRenderTile(ctx, panX + screenMediaBox.x0, panY + screenMediaBox.y0, 480, 272);
	//fz_pixmap* pix = pdfRenderTile(ctx, panX, panY, 480, 272);
	// copy and shift colors
	unsigned int* s = (unsigned int*)pix->samples;
	unsigned int* d = backBuffer;
	const int n = 480*272;
	bk_memcpysr8(d, s, n*4);
	fz_droppixmap(pix);
	bk_memcpy(bounceBuffer, backBuffer, n*4);
	*/

	// render new area

	float nx = float(panX);
        float ny = float(panY);
        clipCoords(nx, ny);
        panX = int(nx);
        panY = int(ny);

	if(xpos_mode || alwaysSetXPos){
	  int* currentPage;
	  if (ctx->pageno%2){ // current page is odd page
	    currentPage = &xpos_odd;
	  }
	  else{ // current page is even page.
	    currentPage = &xpos_even;
	  }
	  
	  if(ctx->rotateLevel%2){ //vetical mode, take panY value
	    *currentPage = panY;
	  }
	  else{ // normal or upsidedown mode, take panX value
	    *currentPage = panX;
	  }
	}

	float bw = fabs(screenMediaBox.x1 - screenMediaBox.x0);
	float bh = fabs(screenMediaBox.y1 - screenMediaBox.y0);
	if (bw > 480)
		bw = 480;
	if (bh > 272)
		bh = 272;
	DEBUGBOOKR("redrawbuffer: before renderTile mem:%d\n",((struct mallinfo)mallinfo()).uordblks);
	fz_pixmap* pix = pdfRenderTile(ctx, panX + screenMediaBox.x0, panY + screenMediaBox.y0, bw, bh);
	DEBUGBOOKR("redrawbuffer: after renderTile mem:%d\n",((struct mallinfo)mallinfo()).uordblks);
	if(!pix){
	if(setSpeed)
	  FZScreen::setSpeed(BKUser::options.pspSpeed);
	
	  return false;
	}
	leftMargin = 0;
	unsigned int* s = (unsigned int*)pix->samples;
	unsigned int* d = bounceBuffer;

	// fill bg area if needed
	if (pix->w < 480 || pix->h < 272) {
		unsigned int *dd = d;
		const unsigned int c = BKUser::options.colorSchemes[BKUser::options.currentScheme].txtBGColor;
		for (int i = 0; i < 480*272; i++) {
			*dd = c;
			++dd;
		}
	}
	// copy rendered tile to bounce buffer for direct viewing
	int pw = pix->w;
	if (pix->w > 480)
		pw = 480;
	int ph = pix->h;
	if (pix->h > 272)
		ph = 272;
	// center tile
	if (pw < 480){
		d += (480 - pw) / 2;
		leftMargin = (480 - pw)/2;
	}
	if (ph < 272)
		d += ((272 - ph) / 2)*480;
	for (int j = 0; j < ph; ++j) {
		bk_memcpysr8(d, s, pw*4);
		d += 480;
		s += pix->w;
	}
	fz_droppixmap(pix);

	setThumbnail( int(fabs(screenMediaBox.x1 - screenMediaBox.x0)), int(fabs(screenMediaBox.y1 - screenMediaBox.y0)), pw, ph, panX, panY );
	/*panX = nx;
	unsigned int* t = bounceBuffer;
	bounceBuffer = backBuffer;
	backBuffer = t;*/
	if(setSpeed)
	  FZScreen::setSpeed(BKUser::options.pspSpeed);
	
	return true;
}

bool BKPDF::redrawBufferIncremental(int newx, int newy, bool setSpeed) {

	int oldx = panX;
	int oldy = panY;
	panX = newx;
	panY = newy;

	if(newx==oldx&&newy==oldy){
	  // no movement actually.
	  return true;
	}

	if (pageError)
	  return false;
	
	if(setSpeed)
	  FZScreen::setSpeed(BKUser::options.pspMenuSpeed);

	int ka_w = (newx>oldx)? (480 - newx + oldx):(480-oldx+newx);
	int ka_h = (newy>oldy)? (272 - newy + oldy):(272-oldy+newy);

	if ((BKUser::options.pageScrollCacheMode == 2 
	     || (BKUser::options.pageScrollCacheMode == 1 && (newx==oldx||newy==oldy)))
	    && ka_w>0 && ka_h>0){
	  // we can use cache for this movement
	  
	  // move reusable area to right position
	  int ka_x = (newx>oldx)?(newx-oldx):0;
	  int ka_y = (newy>oldy)?(newy-oldy):0;
	  
	  int tg_x = ka_x - newx+oldx;
	  int tg_y = ka_y - newy+oldy;
	  
	  if (tg_y < ka_y){
	    //move the first pixel backward first
	    for (int i=0;i<ka_h;i++){
	      memmove(bounceBuffer + (tg_y + i) * 480 + tg_x, bounceBuffer + (ka_y + i) * 480 + ka_x, ka_w*4);
	    }
	  } else{
	    // move the last pixel forward first
	    for (int i=ka_h-1;i>=0;i--){
	      memmove(bounceBuffer + (tg_y + i) * 480 + tg_x, bounceBuffer + (ka_y + i) * 480 + ka_x, ka_w*4);
	    }
	  }
	  
	  // compute new areas (at most 2 areas needed)
	  // area1(x1,y1,w1,h1) is for newy-oldy
	  // area2(x2,y2,w2,h2) is for newx-oldx after newy-oldy
	  int x1, y1, scr_y1, w1, h1, x2, scr_x2, y2, scr_y2, w2, h2;
	  x1 = newx;
	  w1 = 480;
	  y1 = (newy>oldy)?oldy+272:newy;
	  h1 = abs(newy-oldy);

	  //scr_x1 is always 0.
	  scr_y1 = (newy>oldy)?272-h1:0;

	  x2 = (newx>oldx)?oldx+480:newx;
	  w2 = abs(newx-oldx);
	  y2 = (newy>oldy)?newy:oldy;
	  h2 = 272 - h1;
	  
	  scr_x2 = (newx>oldx)?480-w2:0;
	  scr_y2 = (newy>oldy)?0:h1;

	  // Need to re-render cache if either cache is empty or cache misses.
	  if ( cachePageBuffer == NULL 
	       || (h1>0 && !(((x1>=cachePageBuffer->x && (x1+w1)<=(cachePageBuffer->x+cachePageBuffer->w))||(cachePageBuffer->x <= 0 && cachePageBuffer->w >= fabs(screenMediaBox.x1 - screenMediaBox.x0))) && y1>=cachePageBuffer->y && (y1+h1)<=(cachePageBuffer->y+cachePageBuffer->h)))
	       || (w2>0 && !(x2>=cachePageBuffer->x && (x2+w2)<=(cachePageBuffer->x+cachePageBuffer->w) && ((y2>=cachePageBuffer->y && (y2+h2)<=(cachePageBuffer->y+cachePageBuffer->h))||(cachePageBuffer->y <= 0&& cachePageBuffer->h >= fabs(screenMediaBox.y1 - screenMediaBox.y0)))))
	       ){


#ifndef PSP
// 	    // why we are re-rendering cache?
// 	    fprintf(stderr,"debug: cond1: %s\n", cachePageBuffer == NULL ? "true": "false");
// 	    if (cachePageBuffer){
// 	      fprintf(stderr,"debug: cond2: %s\n", (h1>0 && !(((x1>=cachePageBuffer->x && (x1+w1)<=(cachePageBuffer->x+cachePageBuffer->w))||(cachePageBuffer->x <= 0&&cachePageBuffer->w >= fabs(screenMediaBox.x1 - screenMediaBox.x0))) && y1>=cachePageBuffer->y && (y1+h1)<=(cachePageBuffer->y+cachePageBuffer->h)))? "true": "false");

// 	      //fprintf(stderr,"debug: cond2-1: %s\n", h1>0 ?"true": "false");

// 	      fprintf(stderr,"debug: cond3: %s\n", (w2>0 && !(x2>=cachePageBuffer->x && (x2+w2)<=(cachePageBuffer->x+cachePageBuffer->w) && ((y2>=cachePageBuffer->y && (y2+h2)<=(cachePageBuffer->y+cachePageBuffer->h))||(cachePageBuffer->y <= 0&& cachePageBuffer->h >= fabs(screenMediaBox.y1 - screenMediaBox.y0))))) ? "true": "false");
// 	    }
#endif

	    if (cachePageBuffer!=NULL){
	      fz_droppixmap(cachePageBuffer);
	      cachePageBuffer = NULL;
	    }
	    int cachex = newx;
	    int cachey = newy;
	    int cachew = 480;
	    int cacheh = 272;
	    if(BKUser::options.pageScrollCacheMode == 2){
	      cachew = 480*2;
	      cacheh = 272*2;
	      if (newx<oldx)
		cachex = newx-480;
	      if (newy<oldy)
		cachey = newy-272;
	    }
	    else {
	      if(h1>0){
		if(newy>oldy)
		  cachey = oldy+272;
		else
		  cachey = oldy-272;
	      }
	      else{
		if(newx>oldx)
		  cachex = oldx+480;
		else
		  cachex = oldx-480;
	      }
	    }
	    // renderTile(cachex,cachey,cachew,cacheh)
	    if(cachex<0)
	      cachex = 0;
	    if(cachey<0)
	      cachey = 0;
	    if(cachex+cachew>fabs(screenMediaBox.x1 - screenMediaBox.x0))
	      cachew = fz_ceil(fabs(screenMediaBox.x1 - screenMediaBox.x0)-cachex);
	    if(cachey+cacheh>fabs(screenMediaBox.y1 - screenMediaBox.y0))
	      cacheh = fz_ceil(fabs(screenMediaBox.y1 - screenMediaBox.y0)-cachey);
#ifndef PSP
// 	    fprintf(stderr,"re-rendering cache buffer: x: %d, y: %d, w: %d, h: %d\n", cachex, cachey, cachew, cacheh);
#endif
	    
	    cachePageBuffer = pdfRenderTile(ctx, cachex + screenMediaBox.x0, cachey + screenMediaBox.y0, cachew, cacheh);

	    if (cachePageBuffer){
	      unsigned int* s = (unsigned int*)cachePageBuffer->samples;
	      unsigned int n = cachePageBuffer->w * cachePageBuffer->h;
	      for (unsigned int i = 0; i < n; ++i) {
		*s >>= 8;
		++s;
	      }

	      //cachePageBuffer->x -= (int)screenMediaBox.x0+0.1;
	      //cachePageBuffer->y -= (int)screenMediaBox.y0+0.1;
	      cachePageBuffer->x = cachex-(cachePageBuffer->w-cachew);
	      cachePageBuffer->y = cachey-(cachePageBuffer->h-cacheh);

	    }


	    // XXX: do we have to compute the gap?
#if 0
	    if (cachePageBuffer){
	      cachePageBuffer->x = cachex;
	      cachePageBuffer->y = cachey;
	      cachePageBuffer->w = cachew;
	      cachePageBuffer->h = cacheh;
	    }
#endif
	  }
	  // copy correct region from cache to bounceBuffer.
	  if(cachePageBuffer){
	    int pw = 480;
	    int ph = 272;
	    unsigned int* s = (unsigned int*)cachePageBuffer->samples;
	    // copy area1
	    if(h1>0){
	      if (cachePageBuffer->w < 480){
		pw = cachePageBuffer->w;
		const unsigned int c = BKUser::options.colorSchemes[BKUser::options.currentScheme].txtBGColor;
		unsigned int * dd = bounceBuffer+scr_y1*480;
		for(int i =0;i<480*h1;i++){
		  *dd = c;
		  ++dd;
		}
		dd = bounceBuffer+scr_y1*480+(480-cachePageBuffer->w)/2;
		unsigned int* ss = s+cachePageBuffer->w*(y1-cachePageBuffer->y);
		for (int i = 0;i<h1;i++){
		  bk_memcpy(dd, ss, cachePageBuffer->w * 4);
		  dd += 480;
		  ss += cachePageBuffer->w;
		}
	      }
	      else{
		unsigned int * dd = bounceBuffer+scr_y1*480;
		unsigned int* ss = s+cachePageBuffer->w*(y1-cachePageBuffer->y);
		ss += x1-cachePageBuffer->x;
		for (int i = 0;i<h1;i++){
		  bk_memcpy(dd, ss, 480 * 4);
		  dd += 480;
		  ss += cachePageBuffer->w;
		}
	      }
	    }
	    // copy area2
	    if (w2>0){
	      if(cachePageBuffer->h < 272){
		ph = cachePageBuffer->h;
		// h1 should be 0, so scr_y2 is 0
		const unsigned int c = BKUser::options.colorSchemes[BKUser::options.currentScheme].txtBGColor;
		unsigned int * dd = bounceBuffer+scr_x2;
		for (int i =0; i<272;i++){
		  for (int j = 0;j<w2;j++){
		    *(dd+j) = c;
		  }
		  dd += 480;
		}
		
		dd = bounceBuffer + ((272-cachePageBuffer->h)/2)*480 + scr_x2;
		unsigned int* ss = s+(x2-cachePageBuffer->x);
		for (int i = 0;i<h2;i++){
		  bk_memcpy(dd, ss, w2*4);
		  dd += 480;
		  ss += cachePageBuffer->w;
		}
	      }
	      else{
		unsigned int * dd = bounceBuffer+scr_y2*480+scr_x2;
		unsigned int* ss = s+cachePageBuffer->w*(y2-cachePageBuffer->y)+(x2-cachePageBuffer->x);

		for (int i = 0;i<h2;i++){
		  bk_memcpy(dd, ss, w2 * 4);
		  dd += 480;
		  ss += cachePageBuffer->w;
		}
	      }
	    }
	    setThumbnail( int(fabs(screenMediaBox.x1 - screenMediaBox.x0)), int(fabs(screenMediaBox.y1 - screenMediaBox.y0)), pw, ph, panX, panY );
	    if(setSpeed)
	      FZScreen::setSpeed(BKUser::options.pspSpeed);
	    return true;
	  }
	  
	}
	
	// todo: just render it as normal
	float bw = fabs(screenMediaBox.x1 - screenMediaBox.x0);
	float bh = fabs(screenMediaBox.y1 - screenMediaBox.y0);
	if (bw > 480)
	  bw = 480;
	if (bh > 272)
	  bh = 272;
	
	fz_pixmap* pix = pdfRenderTile(ctx, panX + screenMediaBox.x0, panY + screenMediaBox.y0, bw, bh);
	
	if(pix){
	  unsigned int* s = (unsigned int*)pix->samples;
	  unsigned int* d = bounceBuffer;
	  
	  // fill bg area if needed
	  if (pix->w < 480 || pix->h < 272) {
	    unsigned int *dd = d;
	    const unsigned int c = BKUser::options.colorSchemes[BKUser::options.currentScheme].txtBGColor;
	    for (int i = 0; i < 480*272; i++) {
	      *dd = c;
	      ++dd;
	    }
	  }
	  // copy rendered tile to bounce buffer for direct viewing
	  int pw = pix->w;
	  if (pix->w > 480)
	    pw = 480;
	  int ph = pix->h;
	  if (pix->h > 272)
	    ph = 272;
	  // center tile
	  if (pw < 480){
	    d += (480 - pw) / 2;
	  }
	  if (ph < 272)
	    d += ((272 - ph) / 2)*480;
	  for (int j = 0; j < ph; ++j) {
	    bk_memcpysr8(d, s, pw*4);
	    d += 480;
	    s += pix->w;
	  }
	  fz_droppixmap(pix);
	  setThumbnail( int(fabs(screenMediaBox.x1 - screenMediaBox.x0)), int(fabs(screenMediaBox.y1 - screenMediaBox.y0)), pw, ph, panX, panY );
	}
	
	if(setSpeed)
	  FZScreen::setSpeed(BKUser::options.pspSpeed);
	
	return true;
}
void BKPDF::panBuffer(int nx, int ny) {
	if (pageError)
		return;

	if(xpos_mode || alwaysSetXPos){
	  int* currentPage;
	  if (ctx->pageno%2){ // current page is odd page
	    currentPage = &xpos_odd;
	  }
	  else{ // current page is even page.
	    currentPage = &xpos_even;
	  }
	  
	  if(ctx->rotateLevel%2){ //vetical mode, take panY value
	    *currentPage = ny;
	  }
	  else{ // normal or upsidedown mode, take panX value
	    *currentPage = nx;
	  }
	}

	if (BKUser::options.pdfFastScroll && fullPageBuffer != NULL) {
		panX = nx;
		panY = ny;
		redrawBuffer(true);
		return;
	}

	//panX = nx;
	//panY = ny;
	//redrawBuffer(true);
	redrawBufferIncremental(nx, ny, true);


	// incremental pan is disabled, for now
// #if 0
// 	if (ny != panY) {
// 		int dy = ny - panY;
// 		int ady = dy > 0 ? dy : -dy;
// 		int pdfx = panX;
// 		int pdfy = dy > 0 ? panY + 272 : panY + dy;
// 		int pdfw = 480;
// 		int pdfh = ady;
// 		int bx = 0;
// 		int by = dy > 0 ? 272 - dy : 0;
// 		int destDirtyX = 0;
// 		int destDirtyY = dy > 0 ? 0 : ady;
// 		int srcDirtyY = dy > 0 ? ady : 0;
// 		int n = pdfh*pdfw;
// 		// displaced area
// 		bk_memcpy(backBuffer + destDirtyX + destDirtyY*480, bounceBuffer + srcDirtyY*480, 480*(272 - pdfh)*4);
// 		// render new area
// 		fz_pixmap* pix = pdfRenderTile(ctx, pdfx, pdfy, pdfw, pdfh);
// 		// copy and shift colors
// 		bk_memcpysr8(backBuffer + bx + by*480, pix->samples, n*4);
// 		fz_droppixmap(pix);
// 		panY = ny;
// 		unsigned int* t = bounceBuffer;
// 		bounceBuffer = backBuffer;
// 		backBuffer = t;
// 	}
// 	if (nx != panX) {
// 		int dx = nx - panX;
// 		int adx = dx > 0 ? dx : -dx;
// 		int pdfx = dx > 0 ? panX + 480 : panX + dx;
// 		int pdfy = panY;
// 		int pdfw = adx;
// 		int pdfh = 272;
// 		int bx = dx > 0 ? 480 - dx : 0;
// 		int by = 0;

// 		int destDirtyX = dx > 0 ? 0 : adx;
// 		int destDirtyY = 0;
// 		int srcDirtyX = dx > 0 ? adx : 0;
// 		// displaced area
// 		unsigned int* s = bounceBuffer + srcDirtyX;
// 		unsigned int* d = backBuffer + destDirtyX + destDirtyY*480;
// 		int tw = 480 - pdfw;
// 		for (int j = 0; j < 272; ++j) {
// 			bk_memcpy(d, s, tw*4);
// 			s += 480;
// 			d += 480;
// 		}
// 		// render new area
// 		fz_pixmap* pix = pdfRenderTile(ctx, pdfx, pdfy, pdfw, pdfh);
// 		// copy and shift colors
// 		s = (unsigned int*)pix->samples;
// 		d = backBuffer + bx + by*480;
// 		for (int j = 0; j < 272; ++j) {
// 			bk_memcpysr8(d, s, pdfw*4);
// 			d += tw + pdfw;
// 			s += pdfw;
// 		}
// 		fz_droppixmap(pix);
// 		panX = nx;
// 		unsigned int* t = bounceBuffer;
// 		bounceBuffer = backBuffer;
// 		backBuffer = t;
// 	}
// #endif
}

int BKPDF::resume() {
	// mupdf leaves open file descriptors around. they don't survive a suspend.
	//pdfReopenFile();
	return 0;
}

int BKPDF::updateContent() {
	if (lastScrollFlag != BKUser::options.pdfFastScroll){
	  //return BK_CMD_RELOAD;
	  lastScrollFlag = BKUser::options.pdfFastScroll;
	  if (!loadNewPage){
	    if(BKUser::options.pdfFastScroll){
	      if(ctx->zoom < 2.01f && !pageError)
		pdfRenderFullPage(ctx);
	      else{
		if (fullPageBuffer != NULL){
		  fz_droppixmap(fullPageBuffer);
		  fullPageBuffer = NULL;
		}
	      }
	    }
	    else{
	      if (fullPageBuffer != NULL) {
		fz_droppixmap(fullPageBuffer);
		fullPageBuffer = NULL;
	      }
	    }
	  }
	}
	if (loadNewPage) {
		FZScreen::setSpeed(BKUser::options.pspMenuSpeed);
		pageError = pdfLoadPage(ctx) != 0;
		if (! pageError ){
		  retryLoadPageWhenError = true;
		}
		else if (retryLoadPageWhenError){
		  pdfReopenFile();
		  pageError = pdfLoadPage(ctx) != 0;
		  if (! pageError ){
		    retryLoadPageWhenError = true;
		  }
		  else{
		    //pageError = 0;
		    retryLoadPageWhenError = false; //it's unnecessary actually
		    FZScreen::setSpeed(BKUser::options.pspSpeed);
		    return BK_CMD_RELOAD;
		  }
		}
		else{
		  //return 0;
		}
		redrawBuffer();
		loadNewPage = false;
		resetPanXY = false;
		char t[256];
		snprintf(t, 256, "Page %d of %d", ctx->pageno, pdf_getpagecount(ctx->pages));
		setBanner(t);
		FZScreen::setSpeed(BKUser::options.pspSpeed);
		return BK_CMD_MARK_DIRTY;
	}

	/*bannerFrames--;
	if (bannerFrames < 0)
		bannerFrames = 0;*/

	/*
	if (loadNewPage) {
		if (FZScreen::wasSuspended()) {
			FZScreen::clearSuspended();
			PDFContext* oldctx = ctx;
			ctx = pdfOpen((char*)path.c_str());
			if (ctx == 0) {
				// fucked... just implement *proper* error handling some day ok???
				return 0;
			}
			ctx->pageno = oldctx->pageno;
			ctx->zoom = oldctx->zoom;
			ctx->zoomLevel = oldctx->zoomLevel;
			ctx->rotate = oldctx->rotate;
			// remove this if it crashes on resume, but it will leak a ton of ram
			// need*pooled*malloc*NOW
			//pdfClose(oldctx);
			delete oldctx;
		}
		pageError = pdfLoadPage(ctx) != 0;
		if (! pageError ){
		  retryLoadPageWhenError = true;
		}
		redrawBuffer();
		loadNewPage = false;
		char t[256];
		snprintf(t, 256, "Page %d of %d", ctx->pageno, pdf_getpagecount(ctx->pages));
		banner = t;
		bannerFrames = 60;
		return BK_CMD_MARK_DIRTY;
	}
	*/
	return 0;
}

bool BKPDF::isPDF(string& file) {
	char header[4];
	memset((void*)header, 0, 4);
	FILE* f = fopen(file.c_str(), "r");
	if( !f )
		return false;
	fread(header, 4, 1, f);
	fclose(f);
	return header[0] == 0x25 && header[1] == 0x50 && header[2] == 0x44 && header[3] == 0x46;
}

int BKPDF::getFastImageStatus(){
  if (BKUser::options.pageScrollCacheMode == 3 && (fullPageBuffer == NULL))
    return 0;
  return BKUser::options.pageScrollCacheMode;
}

float BKPDF::getCurrentZoom(){
  if(ctx)
    return ctx->zoom;
  return 0.0f;
}
void BKPDF::setZoom(float z){
  if(ctx && z>0.0f){
    ctx->zoom = z;
    int newzl = ctx->zoomLevel;
    if(zoomLevels[newzl] != ctx->zoom){
      newzl = 0;
      while (zoomLevels[newzl]<ctx->zoom){
	newzl++;
      }
      ctx->zoomLevel = newzl;
    }
  }
}

int
BKPDF::getOutlineType(){
  return OUTLINE_PDF;
}

void* 
BKPDF::getOutlines(){
  return ctx?ctx->outline:0;
}


static void setXY(int rotateLevel, float x, float y, float zoom, int& lpanX, int& lpanY, int& lneg_panX, int& lneg_panY){
  switch (rotateLevel){
    case 0: //up
      if(x>=0)
	lpanX = x* zoom;
      if(y>=0)
	lneg_panY = y* zoom;
      break;
    case 1: //right
      if(x>=0)
	lpanY = x* zoom;
      if(y>=0)
	lpanX = y* zoom - 480;
      break;
    case 2: //down
      if(x>=0)
	lneg_panX = x* zoom+480;
      if(y>=0)
	lpanY = y* zoom-272;

      break;
    case 3: //left
      if(x>=0)
	lneg_panY = x* zoom+272;
      if(y>=0)
	lneg_panX = y* zoom;
      break;
    }
}

void
BKPDF::gotoOutline(void* o, bool ignoreZoom){
  pdf_link* link= o?((pdf_outline* )o)->link:NULL;
  if (!link||!ctx){
#ifndef PSP
    fprintf(stderr, "GOTOOUTLINE: no link or ctx. link:%p ctx:%p\n",link, ctx);
#endif    
    return;
  }
  int pageno = pdf_getpagenumberfromreference(ctx->pages,link->dest);
  
  if (pageno<0){
#ifndef PSP
    fprintf(stderr, "GOTOOUTLINE: cannot get page number.oid: %d\n",link->dest->u.r.oid);
#endif    
    return;
  }
#ifndef PSP
  fprintf(stderr, "GOTOOUTLINE: page: %d, pos_type: %d, %f %f %f %f\n",pageno, link->pos_type, link->x, link->y, link->z, link->w);
#endif    

  // let's get w/h of the target page...
  fz_obj* targetPage = pdf_getpageobject(ctx->pages, pageno);
  fz_obj* obj = fz_dictgets(targetPage, "CropBox");
  bool getpagewhfailed = false;
  fz_rect bbox;
  float pagew, pageh;
  if (!obj)
    obj = fz_dictgets(targetPage, "MediaBox");
  fz_error error = pdf_resolve(&obj, ctx->xref);
  if (error)
    getpagewhfailed = true;
  else if (!fz_isarray(obj)){
    fz_dropobj(obj);
    getpagewhfailed = true;
  }
  else{
    bbox = pdf_torect(obj);
    fz_dropobj(obj);
    pagew = fabs(bbox.x1-bbox.x0);
    pageh = fabs(bbox.y1-bbox.y0);
  }
  
#ifndef PSP
  fprintf(stderr, "GET_PAGE_WH: failed? %s. w: %f h: %f\n",getpagewhfailed?"YES":"NO", pagew, pageh);
#endif
  

  setCurrentPage(pageno+1);
  switch (link->pos_type){
  case 0: //XYZ
    if(!ignoreZoom && link->z>0){
      setZoom(link->z);
    }
    if(ignoreZoom&&BKUser::options.t_ignore_x)
      setXY(ctx->rotateLevel, -1, link->y, ctx->zoom, panX, panY, neg_panX, neg_panY);
    else
      setXY(ctx->rotateLevel, link->x, link->y, ctx->zoom, panX, panY, neg_panX, neg_panY);
    break;
  case 1: //Fit
  case 5: //FitB
    if(!ignoreZoom && !getpagewhfailed){
      float lzoom;
      if(ctx->rotateLevel%2){ // 1 or 3
	lzoom = (480/pageh) > (272/pagew) ? (272/pagew):(480/pageh);
      }
      else{ // 0 or 2
	lzoom = (272/pageh) > (480/pagew) ? (480/pagew):(272/pageh);
      }
      setZoom(lzoom);
    }
    if(ignoreZoom&&BKUser::options.t_ignore_x)
      setXY(ctx->rotateLevel, -1, pageh, ctx->zoom, panX, panY, neg_panX, neg_panY);
    else
      setXY(ctx->rotateLevel, 0, pageh, ctx->zoom, panX, panY, neg_panX, neg_panY);
    break;
  case 2: //FitH
  case 6: //FitBH
    if(!ignoreZoom && !getpagewhfailed){
      float lzoom;
      if(ctx->rotateLevel%2){ // 1 or 3
	lzoom = 272/pagew;
      }
      else{ // 0 or 2
	lzoom = 480/pagew;
      }
      setZoom(lzoom);
    }
    if(ignoreZoom&&BKUser::options.t_ignore_x)
      setXY(ctx->rotateLevel, -1, link->y, ctx->zoom, panX, panY, neg_panX, neg_panY);
    else
      setXY(ctx->rotateLevel, link->x, link->y, ctx->zoom, panX, panY, neg_panX, neg_panY);
    break;
  case 3: //FitV
  case 7: //FitBV
    if(!ignoreZoom && !getpagewhfailed){
      float lzoom;
      if(ctx->rotateLevel%2){ // 1 or 3
	lzoom = 480/pageh;
      }
      else{ // 0 or 2
	lzoom = 272/pageh;
      }
      setZoom(lzoom);
    }
    if(ignoreZoom&&BKUser::options.t_ignore_x)
      setXY(ctx->rotateLevel, -1, link->y, ctx->zoom, panX, panY, neg_panX, neg_panY);
    else
      setXY(ctx->rotateLevel, link->x, link->y, ctx->zoom, panX, panY, neg_panX, neg_panY);
    break;
  case 4: //FitR
    if(!ignoreZoom && !getpagewhfailed){
      float lzoom;
      if(ctx->rotateLevel%2){ // 1 or 3
	lzoom = 480/fabs(link->y-link->z) < 272/fabs(link->w-link->x) ? 480/fabs(link->y-link->z) : 272/fabs(link->w-link->x);
      }
      else{ // 0 or 2
	lzoom = 272/fabs(link->y-link->z) < 480/fabs(link->w-link->x) ? 272/fabs(link->y-link->z) : 480/fabs(link->w-link->x);
      }
      setZoom(lzoom);
    }
    if(ignoreZoom&&BKUser::options.t_ignore_x)
      setXY(ctx->rotateLevel, -1, link->y, ctx->zoom, panX, panY, neg_panX, neg_panY);
    else
      setXY(ctx->rotateLevel, link->x, link->y, ctx->zoom, panX, panY, neg_panX, neg_panY);
    break;

  default:

#ifndef PSP
    fprintf(stderr, "GOTOOUTLINE: unimplemented page position: %d\n",link->pos_type);
#endif
    break;
  }
  loadNewPage = true;
  resetPanXY = false;

  //updateContent();

}
