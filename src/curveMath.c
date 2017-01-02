#include "curveMath.h"
#include <string.h>

int pointZZ_pEqual(const PointZZ_p * op1, const PointZZ_p * op2) {
    // check x coords
    if(mpz_cmp(op1->x, op2->x) != 0) {
        return 0;
    }
    // check y coords
    if(mpz_cmp(op1->y, op2->y) != 0) {
        return 0;
    }

    return 1;
}


void pointZZ_pDouble(PointZZ_p * rop, const PointZZ_p * op, const CurveZZ_p * curve) {
    mpz_t numer, denom, lambda;
    mpz_inits(numer, denom, lambda, NULL);

    // calculate lambda
    mpz_mul(numer, op->x, op->x);
    mpz_mul_ui(numer, numer, 3);
    mpz_add(numer, numer, curve->a);
    mpz_mod(numer, numer, curve->p);
    mpz_mul_ui(denom, op->y, 2);
    mpz_mod(denom, denom, curve->p);
    mpz_invert(denom, denom, curve->p);  // TODO check status
    mpz_mul(lambda, numer, denom);
    mpz_mod(lambda, lambda, curve->p);

    // calculate resulting x coord
    mpz_mul(rop->x, lambda, lambda);
    mpz_sub(rop->x, rop->x, op->x);
    mpz_sub(rop->x, rop->x, op->x);
    mpz_mod(rop->x, rop->x, curve->p);

    //calculate resulting y coord
    mpz_sub(rop->y, op->x, rop->x);
    mpz_mul(rop->y, lambda, rop->y);
    mpz_sub(rop->y, rop->y, op->y);
    mpz_mod(rop->y, rop->y, curve->p);

    mpz_clears(numer, denom, lambda, NULL);
}


void pointZZ_pXDouble(PointZZ_pX * rop, const PointZZ_pX * op, const CurveZZ_pX * curve) {
    fq_poly_t lambda, tmp, G, T;
    fq_poly_init2(lambda, curve->degree, curve->ctx);
    fq_poly_init2(tmp, curve->degree, curve->ctx);
    fq_poly_init2(G, curve->degree, curve->ctx);
    fq_poly_init2(T, curve->degree, curve->ctx);

    // calculate lambda
    fq_poly_xgcd_euclidean(G, tmp, T, op->x, curve->pt, curve->ctx);
    fq_poly_mulmod(lambda, op->y, tmp, curve->pt, curve->ctx);
    fq_poly_add(lambda, lambda, op->x, curve->ctx);

    // calculate x coordinate
    fq_poly_mulmod(rop->x, lambda, lambda, curve->pt, curve->ctx);
    fq_poly_add(rop->x, rop->x, lambda, curve->ctx);
    fq_poly_add(rop->x, rop->x, curve->a, curve->ctx);

    // calculate y coordinate
    fq_poly_mulmod(rop->y, op->x, op->x, curve->pt, curve->ctx);
    fq_poly_mulmod(tmp, lambda, rop->x, curve->pt, curve->ctx);
    fq_poly_add(rop->y, rop->y, tmp, curve->ctx);
    fq_poly_add(rop->y, rop->y, rop->x, curve->ctx);

    // clear allocated data
    fq_poly_clear(T, curve->ctx);
    fq_poly_clear(G, curve->ctx);
    fq_poly_clear(tmp, curve->ctx);
    fq_poly_clear(lambda, curve->ctx);
}


void pointZZ_pAdd(PointZZ_p * rop, const PointZZ_p * op1, const PointZZ_p * op2, const CurveZZ_p * curve) {
    mpz_t xdiff, ydiff, lambda;
    mpz_inits(xdiff, ydiff, lambda, NULL);

    // calculate lambda
    mpz_sub(ydiff, op2->y, op1->y);
    mpz_mod(ydiff, ydiff, curve->p);
    mpz_sub(xdiff, op2->x, op1->x);
    mpz_mod(xdiff, xdiff, curve->p);
    mpz_invert(xdiff, xdiff, curve->p);  // TODO check status
    mpz_mul(lambda, ydiff, xdiff);
    mpz_mod(lambda, lambda, curve->p);

    // calculate resulting x coord
    mpz_mul(rop->x, lambda, lambda);
    mpz_sub(rop->x, rop->x, op1->x);
    mpz_sub(rop->x, rop->x, op2->x);
    mpz_mod(rop->x, rop->x, curve->p);

    //calculate resulting y coord
    mpz_sub(rop->y, op1->x, rop->x);
    mpz_mul(rop->y, lambda, rop->y);
    mpz_sub(rop->y, rop->y, op1->y);
    mpz_mod(rop->y, rop->y, curve->p);

    mpz_clears(xdiff, ydiff, lambda, NULL);
}


void pointZZ_pXAdd(PointZZ_pX * rop, const PointZZ_pX * op1, const PointZZ_pX * op2, const CurveZZ_pX * curve) {
    fq_poly_t lambda, tmp1, tmp2, G, T;
    fq_poly_init2(lambda, curve->degree, curve->ctx);
    fq_poly_init2(tmp1, curve->degree, curve->ctx);
    fq_poly_init2(tmp2, curve->degree, curve->ctx);
    fq_poly_init2(G, curve->degree, curve->ctx);
    fq_poly_init2(T, curve->degree, curve->ctx);

    // calculate lambda
    fq_poly_add(tmp1, op1->y, op2->y, curve->ctx);
    fq_poly_add(tmp2, op1->x, op2->x, curve->ctx);
    fq_poly_xgcd_euclidean(G, tmp2, T, tmp2, curve->pt, curve->ctx);
    fq_poly_mulmod(lambda, tmp1, tmp2, curve->pt, curve->ctx);

    // calculate x coordinate
    fq_poly_mulmod(rop->x, lambda, lambda, curve->pt, curve->ctx);
    fq_poly_add(rop->x, rop->x, lambda, curve->ctx);
    fq_poly_add(rop->x, rop->x, op1->x, curve->ctx);
    fq_poly_add(rop->x, rop->x, op2->x, curve->ctx);
    fq_poly_add(rop->x, rop->x, curve->a, curve->ctx);

    // calculate y coordinate
    fq_poly_add(rop->y, op1->x, rop->x, curve->ctx);
    fq_poly_mulmod(rop->y, rop->y, lambda, curve->pt, curve->ctx);
    fq_poly_add(rop->y, rop->y, rop->x, curve->ctx);
    fq_poly_add(rop->y, rop->y, op1->y, curve->ctx);

    // clear allocated data
    fq_poly_clear(T, curve->ctx);
    fq_poly_clear(G, curve->ctx);
    fq_poly_clear(tmp2, curve->ctx);
    fq_poly_clear(tmp1, curve->ctx);
    fq_poly_clear(lambda, curve->ctx);
}


void pointZZ_pMul(PointZZ_p * rop, const PointZZ_p * point, const mpz_t scalar, const CurveZZ_p * curve) {
    PointZZ_p R0, R1, tmp;
    mpz_inits(R1.x, R1.y, tmp.x, tmp.y, NULL);
    mpz_init_set(R0.x, point->x);
    mpz_init_set(R0.y, point->y);
    pointZZ_pDouble(&R1, point, curve);

    char * dbits = mpz_get_str(NULL, 2, scalar);
    int i = 1;

    while(dbits[i] != '\0') {
        if(dbits[i] == '0') {
            mpz_set(tmp.x, R1.x);
            mpz_set(tmp.y, R1.y);
            pointZZ_pAdd(&R1, &R0, &tmp, curve);
            mpz_set(tmp.x, R0.x);
            mpz_set(tmp.y, R0.y);
            pointZZ_pDouble(&R0, &tmp, curve);
        }
        else {
            mpz_set(tmp.x, R0.x);
            mpz_set(tmp.y, R0.y);
            pointZZ_pAdd(&R0, &R1, &tmp, curve);
            mpz_set(tmp.x, R1.x);
            mpz_set(tmp.y, R1.y);
            pointZZ_pDouble(&R1, &tmp, curve);
        }

        i++;
    }

    mpz_init_set(rop->x, R0.x);
    mpz_init_set(rop->y, R0.y);
    mpz_clears(R0.x, R0.y, R1.x, R1.y, tmp.x, tmp.y, NULL);
}


void pointZZ_pXMul(PointZZ_pX * rop, const PointZZ_pX * point, const mpz_t scalar, const CurveZZ_pX * curve) {
    PointZZ_pX R0, R1, tmp;
    fq_poly_init2(rop->x, curve->degree, curve->ctx);
    fq_poly_init2(rop->y, curve->degree, curve->ctx);
    fq_poly_init2(R0.x, curve->degree, curve->ctx);
    fq_poly_init2(R0.y, curve->degree, curve->ctx);
    fq_poly_init2(R1.x, curve->degree, curve->ctx);
    fq_poly_init2(R1.y, curve->degree, curve->ctx);
    fq_poly_init2(tmp.x, curve->degree, curve->ctx);
    fq_poly_init2(tmp.y, curve->degree, curve->ctx);

    fq_poly_set(R0.x, point->x, curve->ctx);
    fq_poly_set(R0.y, point->y, curve->ctx);
    pointZZ_pXDouble(&R1, point, curve);

    char * dbits = mpz_get_str(NULL, 2, scalar);
    int i = 1;

    while(dbits[i] != '\0') {
        if(dbits[i] == '0') {
            fq_poly_set(tmp.x, R1.x, curve->ctx);
            fq_poly_set(tmp.y, R1.y, curve->ctx);
            pointZZ_pXAdd(&R1, &R0, &tmp, curve);
            fq_poly_set(tmp.x, R0.x, curve->ctx);
            fq_poly_set(tmp.y, R0.y, curve->ctx);
            pointZZ_pXDouble(&R0, &tmp, curve);
        }
        else {
            fq_poly_set(tmp.x, R0.x, curve->ctx);
            fq_poly_set(tmp.y, R0.y, curve->ctx);
            pointZZ_pXAdd(&R0, &R1, &tmp, curve);
            fq_poly_set(tmp.x, R1.x, curve->ctx);
            fq_poly_set(tmp.y, R1.y, curve->ctx);
            pointZZ_pXDouble(&R1, &tmp, curve);
        }

        i++;
    }

    fq_poly_set(rop->x, R0.x, curve->ctx);
    fq_poly_set(rop->y, R0.y, curve->ctx);

    fq_poly_clear(R0.x, curve->ctx);
    fq_poly_clear(R0.y, curve->ctx);
    fq_poly_clear(R1.x, curve->ctx);
    fq_poly_clear(R1.y, curve->ctx);
    fq_poly_clear(tmp.x, curve->ctx);
    fq_poly_clear(tmp.y, curve->ctx);
}


/******************************************************************************
 PYTHON BINDINGS
 ******************************************************************************/
static PyObject * curvemath_mul(PyObject *self, PyObject *args) {
    char * x, * y, * d, * curveName;

    if (!PyArg_ParseTuple(args, "ssss", &x, &y, &d, &curveName)) {
        return NULL;
    }

    PointZZ_p * point = buildPointZZ_p(x, y, 10);
    CurveZZ_p * curve;
    PointZZ_p result;
    mpz_t scalar;

    if(strcmp(curveName, "P192") == 0) { curve = buildP192(); }
    else if(strcmp(curveName, "P224") == 0) { curve = buildP224(); }
    else if(strcmp(curveName, "P256") == 0) { curve = buildP256(); }
    else if(strcmp(curveName, "P384") == 0) { curve = buildP384(); }
    else if(strcmp(curveName, "P521") == 0) { curve = buildP521(); }
    else if(strcmp(curveName, "secp256k1") == 0) { curve = buildSecp256k1(); }
    else { return NULL; }

    mpz_init_set_str(scalar, d, 10);

    pointZZ_pMul(&result, point, scalar, curve);
    char * resultX = mpz_get_str(NULL, 10, result.x);
    char * resultY = mpz_get_str(NULL, 10, result.y);

    destroyPointZZ_p(point);
    destroyCurveZZ_p(curve);
    mpz_clears(result.x, result.y, scalar, NULL);

    return Py_BuildValue("ss", resultX, resultY);
}

static PyObject * curvemath_add(PyObject *self, PyObject *args) {
    char * px, * py, * qx, * qy, * curveName;

    if (!PyArg_ParseTuple(args, "sssss", &px, &py, &qx, &qy, &curveName)) {
        return NULL;
    }

    PointZZ_p * p = buildPointZZ_p(px, py, 10);
    PointZZ_p * q = buildPointZZ_p(qx, qy, 10);
    CurveZZ_p * curve;
    PointZZ_p result;
    mpz_inits(result.x, result.y, NULL);

    if(strcmp(curveName, "P192") == 0) { curve = buildP192(); }
    else if(strcmp(curveName, "P224") == 0) { curve = buildP224(); }
    else if(strcmp(curveName, "P256") == 0) { curve = buildP256(); }
    else if(strcmp(curveName, "P384") == 0) { curve = buildP384(); }
    else if(strcmp(curveName, "P521") == 0) { curve = buildP521(); }
    else if(strcmp(curveName, "secp256k1") == 0) { curve = buildSecp256k1(); }
    else { return NULL; }

    if(pointZZ_pEqual(p, q)) {
        pointZZ_pDouble(&result, p, curve);
    }
    else {
        pointZZ_pAdd(&result, p, q, curve);
    }

    char * resultX = mpz_get_str(NULL, 10, result.x);
    char * resultY = mpz_get_str(NULL, 10, result.y);

    destroyPointZZ_p(p);
    destroyPointZZ_p(q);
    destroyCurveZZ_p(curve);
    mpz_clears(result.x, result.y, NULL);

    return Py_BuildValue("ss", resultX, resultY);
}


static PyMethodDef curvemath__methods__[] = {
    {"mul", curvemath_mul, METH_VARARGS, "Multiply a curve point by an integer scalar."},
    {"add", curvemath_add, METH_VARARGS, "Add two points on a curve."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "curvemath",            /* m_name */
    NULL,                   /* m_doc */
    -1,                     /* m_size */
    curvemath__methods__,   /* m_methods */
    NULL,                   /* m_reload */
    NULL,                   /* m_traverse */
    NULL,                   /* m_clear */
    NULL,                   /* m_free */
};


PyMODINIT_FUNC PyInit_curvemath(void) {
    PyObject * m = PyModule_Create(&moduledef);
    return m;
}


#else
PyMODINIT_FUNC initcurvemath(void) {
    Py_InitModule("curvemath", curvemath__methods__);
}
#endif
