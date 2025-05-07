#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <quickjs.h>
#include <string.h>

static void raise(JSContext* ctx) {
	JSValue v = JS_GetException(ctx);

	const char* msg = JS_ToCString(ctx, v);
	fprintf(stderr, "JS exception: %s\n", msg);
	exit(1);
}
int main(int argc, char** argv) {
	JSRuntime* rt = JS_NewRuntime();
	JSContext* ctx = JS_NewContextRaw(rt);
    JS_AddIntrinsicBaseObjects(ctx);
    JS_AddIntrinsicDate(ctx);
    JS_AddIntrinsicEval(ctx);
    JS_AddIntrinsicStringNormalize(ctx);
    JS_AddIntrinsicRegExp(ctx);
    JS_AddIntrinsicJSON(ctx);
    JS_AddIntrinsicProxy(ctx);
    JS_AddIntrinsicMapSet(ctx);
    JS_AddIntrinsicTypedArrays(ctx);
    JS_AddIntrinsicPromise(ctx);
    JS_AddIntrinsicBigInt(ctx);

    if (argc != 2) {
    	printf("Usage: %s <source>\n", argv[0]);
    	return 1;
    }

	JSValue f = JS_NewCFunction(ctx, [] (JSContext* ctx, JSValueConst thisval, int argc, JSValueConst* argv) -> JSValue {
		if (argc != 1) return JS_UNDEFINED;
		const char* msg = JS_ToCString(ctx, argv[0]);
		printf("%s\n", msg);
		return JS_UNDEFINED;
	}, "print", 1);
    JS_SetPropertyStr(ctx, JS_GetGlobalObject(ctx), "print", f);
	JSValue x = JS_NewCFunction(ctx, [] (JSContext* ctx, JSValueConst thisval, int argc, JSValueConst* argv) -> JSValue {
		if (argc != 1) return JS_UNDEFINED;
		size_t psize;
		uint8_t* mem = JS_GetArrayBuffer(ctx, &psize, argv[0]);
		if (!mem) return JS_UNDEFINED;
		void* cpy = malloc(psize);
		memcpy(cpy, mem, psize);

		JSValue v = JS_ReadObject(ctx, (uint8_t*)cpy, psize, JS_READ_OBJ_BYTECODE);
	    if (JS_IsException(v)) {
	    	raise(ctx);
	    }
		v = JS_EvalFunction(ctx, v);
	    if (JS_IsException(v)) {
	    	raise(ctx);
	    }
	    return v;
	}, "loadall", 0);
    JS_SetPropertyStr(ctx, JS_GetGlobalObject(ctx), "loadall", x);

	FILE* prg = fopen(argv[1], "rb");
	fseek(prg, 0, SEEK_END);
	long offs = ftell(prg);
	void* code = malloc(offs+1);
	((char*)code)[offs] = 0;
	fseek(prg, 0, SEEK_SET);
	fread(code, offs, 1, prg);
	fclose(prg);

    JSValue v = JS_Eval(ctx, (char*)code, offs, "", 0);
    if (JS_IsException(v)) {
    	raise(ctx);
    }
}

