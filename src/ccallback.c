/*
** FFI C callback handling
** tea_ccallback.c
*/

#include <stdlib.h>
#include <ffi.h>
#include <tea.h>
#include "teax.h"
#include "tea_ffi.h"
#include "cconv.h"
#include "ccallback.h"

typedef struct
{
    tea_State* T;
    CData* cd;
    ffi_cif cif;
    ffi_type* args[MAX_FUNC_ARGS];
} FFIClosureContext;

static void ffi_callback_handler(ffi_cif* cif, void* result, void** values, void* data)
{
    FFIClosureContext* ctx = data;
    CFunc* func = ctx->cd->ct->func;
    int i;
    (void)cif;
    tea_get_fieldp(ctx->T, TEA_REGISTRY_INDEX, ctx->cd);
    tea_get_key(ctx->T, -1, "callback");
    tea_remove(ctx->T, -2);
    for(i = 0; i < func->narg; i++)
    {
        cconv_tea_cdata(ctx->T, func->args[i], values[i]);
    }
    if(tea_pcall(ctx->T, func->narg) != 0)
    {
        tea_pop(ctx->T, 1);
        return;
    }
    if(func->rtype->type != CTYPE_VOID)
    {
        cconv_cdata_tea(ctx->T, func->rtype, result, -1, true);
        tea_pop(ctx->T, 1);
    }
}

bool ccallback_new(tea_State* T, CType* ct, CData* cd, int funcidx)
{
    FFIClosureContext* ctx;
    ffi_closure* closure;
    void* code;
    int i;
    ffi_status status;
    if(ct->type != CTYPE_FUNC || !tea_is_function(T, funcidx))
        return false;
    ctx = calloc(1, sizeof(*ctx)); ctx->T = T; ctx->cd = cd;
    for(i = 0; i < ct->func->narg; i++)
    {
        ctx->args[i] = ctype_ft(ct->func->args[i]);
    }
    status = ffi_prep_cif(&ctx->cif, FFI_DEFAULT_ABI, ct->func->narg, ctype_ft(ct->func->rtype), ctx->args);
    if(status) tea_error(T, "ffi_prep_cif callback fail: %d", status);
    closure = ffi_closure_alloc(sizeof(*closure), &code);
    if(!closure) tea_error(T, "cannot allocate ffi callback closure");
    status = ffi_prep_closure_loc(closure, &ctx->cif, ffi_callback_handler, ctx, code);
    if(status) tea_error(T, "ffi_prep_closure callback fail: %d", status);
    cd->closure = ctx; cdata_ptr_set(cd, code);

    tea_get_fieldp(T, TEA_REGISTRY_INDEX, cd);
    tea_push_value(T, funcidx);
    tea_set_key(T, -2, "callback");
    tea_pop(T, 1);

    return true;
}

void ccallback_free(CData* cd)
{
    if(cd->closure)
    {
        FFIClosureContext* ctx = cd->closure;
        ffi_closure_free(cd->ptr);
        free(ctx);
        cd->closure = NULL;
        cd->ptr = NULL;
    }
}