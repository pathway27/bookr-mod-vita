#include <fitz.h>
#include <mupdf.h>

struct stuff
{
	fz_obj *resources;
	fz_obj *mediabox;
	fz_obj *cropbox;
	fz_obj *rotate;
};

static fz_error *
loadpagetree(pdf_xref *xref, pdf_pagetree *pages,
	struct stuff inherit, fz_obj *obj, fz_obj *ref, int index, fz_obj *super)
{
	fz_error *error;
	fz_obj *type;
	fz_obj *kids;
	fz_obj *kref, *kobj;
	fz_obj *inh;
	int i;

	type = fz_dictgets(obj, "Type");

	if (strcmp(fz_toname(type), "Page") == 0)
	{
		pages->indexes[pages->cursor] = index;
		pages->supers[pages->cursor] = fz_keepobj(super);
		pages->cursor++;
	}

	else if (strcmp(fz_toname(type), "Pages") == 0)
	{
		if (inherit.resources && !fz_dictgets(obj, "Resources"))
		{
			pdf_logpage("inherit resources (%d)\n", 0);
			error = fz_dictputs(obj, "Resources", inherit.resources);
			if (error) return error;
		}

		if (inherit.mediabox && !fz_dictgets(obj, "MediaBox"))
		{
			pdf_logpage("inherit mediabox (%d)\n", 0);
			error = fz_dictputs(obj, "MediaBox", inherit.mediabox);
			if (error) return error;
		}

		if (inherit.cropbox && !fz_dictgets(obj, "CropBox"))
		{
			pdf_logpage("inherit cropbox (%d)\n", 0);
			error = fz_dictputs(obj, "CropBox", inherit.cropbox);
			if (error) return error;
		}

		if (inherit.rotate && !fz_dictgets(obj, "Rotate"))
		{
			pdf_logpage("inherit rotate (%d)\n", 0);
			error = fz_dictputs(obj, "Rotate", inherit.rotate);
			if (error) return error;
		}

		inh = fz_dictgets(obj, "Resources");
		if (inh) inherit.resources = inh;

		inh = fz_dictgets(obj, "MediaBox");
		if (inh) inherit.mediabox = inh;

		inh = fz_dictgets(obj, "CropBox");
		if (inh) inherit.cropbox = inh;

		inh = fz_dictgets(obj, "Rotate");
		if (inh) inherit.rotate = inh;

		kids = fz_dictgets(obj, "Kids");
		error = pdf_resolve(&kids, xref);
		if (error)
			return error;

		pdf_logpage("subtree %d {\n", fz_arraylen(kids));

		for (i = 0; i < fz_arraylen(kids); i++)
		{
			kref = fz_arrayget(kids, i);

			error = pdf_loadindirect(&kobj, xref, kref);
			if (error) { fz_dropobj(kids); return error; }

			error = loadpagetree(xref, pages, inherit, kobj, kref, i, obj);
			fz_dropobj(kobj);
			pdf_uncacheobject(xref, fz_tonum(kref), fz_togen(kref));
			if (error) { fz_dropobj(kids); return error; }
		}

		fz_dropobj(kids);

		pdf_logpage("}\n");
	}

	return nil;
}



void
pdf_debugpagetree(pdf_pagetree *pages)
{
	printf("<<\n  /Type /Pages\n  /Count %d\n  /Kids [\n", pages->count);
		printf("    ");
	if (pages->pref)
		fz_debugobj(pages->pref);
	printf("  ]\n>>\n");
}

fz_error *
pdf_loadpagetree(pdf_pagetree **pp, pdf_xref *xref)
{
	fz_error *error;
	struct stuff inherit;
	pdf_pagetree *p = nil;
	fz_obj *catalog = nil;
	fz_obj *pages = nil;
	fz_obj *trailer;
	fz_obj *ref;
	int count;

	inherit.resources = nil;
	inherit.mediabox = nil;
	inherit.cropbox = nil;
	inherit.rotate = nil;

	trailer = xref->trailer;

	ref = fz_dictgets(trailer, "Root");
	error = pdf_loadindirect(&catalog, xref, ref);
	if (error) goto cleanup;

	ref = fz_dictgets(catalog, "Pages");
	error = pdf_loadindirect(&pages, xref, ref);
	if (error) goto cleanup;

	ref = fz_dictgets(pages, "Count");
	count = fz_toint(ref);

	p = fz_malloc(sizeof(pdf_pagetree));
	if (!p) { error = fz_outofmem; goto cleanup; }

	pdf_logpage("load pagetree %p {\n", p);
	pdf_logpage("count %d\n", count);

	p->pref = nil;
	p->pobj = nil;
	p->count = count;
	p->cursor = 0;
	p->indexes = nil;
	p->supers = nil;

	p->indexes = fz_malloc(sizeof(int) * count);
	if (!p->indexes) { error = fz_outofmem; goto cleanup; }

	p->supers = fz_malloc(sizeof(fz_obj*) * count);
	if (!p->supers) { error = fz_outofmem; goto cleanup; }

	int i;
	for (i = 0; i < p->count; i++) {
		p->supers[i] = nil;
	}

	error = loadpagetree(xref, p, inherit, pages, ref, 0, pages);
	if (error) goto cleanup;

	fz_dropobj(pages);
	fz_dropobj(catalog);

	pdf_logpage("}\n", count);

	*pp = p;
	return nil;

cleanup:
	if (pages) fz_dropobj(pages);
	if (catalog) fz_dropobj(catalog);
	if (p) {
		fz_free(p->pref);
		fz_free(p->pobj);
		fz_free(p);
	}
	return nil;
}

int
pdf_getpagecount(pdf_pagetree *pages)
{
	return pages->count;
}

fz_obj *
pdf_getpageobject(pdf_pagetree *pages, int p, pdf_xref *xref)
{
	if (p < 0 || p >= pages->count)
		return nil;

	fz_error *error;
	fz_obj *kids;
	fz_obj *kref, *kobj;
	fz_obj *type;
	struct stuff inherit;

	kids = fz_dictgets(pages->supers[p], "Kids");
	error = pdf_resolve(&kids, xref);
	if (error)
		return nil;

	pdf_logpage("subtree %d {\n", fz_arraylen(kids));

	if (pages->indexes[p] < 0 || pages->indexes[p] >= fz_arraylen(kids)) {
		fz_dropobj(kids);
		return nil;
	}

	kref = fz_arrayget(kids, pages->indexes[p]);

	error = pdf_loadindirect(&kobj, xref, kref);
	if (error) { fz_dropobj(kids); return nil; }

	type = fz_dictgets(kobj, "Type");

	if (strcmp(fz_toname(type), "Page") == 0)
	{
		inherit.resources = fz_dictgets(pages->supers[p], "Resources");
		inherit.mediabox = fz_dictgets(pages->supers[p], "MediaBox");
		inherit.cropbox = fz_dictgets(pages->supers[p], "CropBox");
		inherit.rotate = fz_dictgets(pages->supers[p], "Rotate");

		if (inherit.resources && !fz_dictgets(kobj, "Resources"))
		{
			pdf_logpage("inherit resources (%d)\n", 0);
			error = fz_dictputs(kobj, "Resources", inherit.resources);
			if (error) goto done;
		}

		if (inherit.mediabox && !fz_dictgets(kobj, "MediaBox"))
		{
			pdf_logpage("inherit mediabox (%d)\n", 0);
			error = fz_dictputs(kobj, "MediaBox", inherit.mediabox);
			if (error) goto done;
		}

		if (inherit.cropbox && !fz_dictgets(kobj, "CropBox"))
		{
			pdf_logpage("inherit cropbox (%d)\n", 0);
			error = fz_dictputs(kobj, "CropBox", inherit.cropbox);
			if (error) goto done;
		}

		if (inherit.rotate && !fz_dictgets(kobj, "Rotate"))
		{
			pdf_logpage("inherit rotate (%d)\n", 0);
			error = fz_dictputs(kobj, "Rotate", inherit.rotate);
			if (error) goto done;
		}

		if (pages->pref)
			fz_dropobj(pages->pref);
		if (pages->pobj)
			fz_dropobj(pages->pobj);

		pages->pref = fz_keepobj(kref);
		pages->pobj = fz_keepobj(kobj);
	}

done:
	pdf_uncacheobject(xref, fz_tonum(kref), fz_togen(kref));
	fz_dropobj(kobj);
	if (error) { fz_dropobj(kids); return nil; }

	fz_dropobj(kids);

	pdf_logpage("}\n");

	return pages->pobj;
}

void
pdf_droppagetree(pdf_pagetree *pages)
{
	int i;

	pdf_logpage("drop pagetree %p\n", pages);

	for (i = 0; i < pages->count; i++) {
		if (pages->supers[i])
			fz_dropobj(pages->supers[i]);
	}

	if (pages->pref) {
		fz_dropobj(pages->pref);
	}
	if (pages->pobj) {
		fz_dropobj(pages->pobj);
	}
	if (pages->indexes)
		fz_free(pages->indexes);
	fz_free(pages->supers);
	fz_free(pages);
}

