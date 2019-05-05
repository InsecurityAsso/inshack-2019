#ifndef __MACRO_H__
#define __MACRO_H__
/*
 * Base constants
 *
 * /!\ DO NOT CHANGE THESE VALUES /!\
 */
#define OK  0
#define NOK 1
/*
 * Conditionnal branching operations for success and failure
 */
#define IF_NOK(cond)        if((cond)!=OK)
#define IF_OK(cond)         if((cond)==OK)
/*
 * Conditionnal branching operations with affectation for success and failure
 */
#define IF_NOK_SET(cond)    if((nok=(cond))!=OK)
#define IF_OK_SET(cond)     if((ok=(cond))==OK)
/*
 * Branching operations error, success or failure
 */
#define ERR_GOTO(error, label)  err=error; goto label
#define OK_GOTO(label)          ERR_GOTO(OK, label)
#define NOK_GOTO(label)         ERR_GOTO(NOK, label)
/*
 * Arithmetic min() and max() function (without type assertion...)
 */
#define MAX(a, b)       ((a)<(b)?(b):(a))
#define MIN(a, b)       ((a)>(b)?(b):(a))
#define PRINTABLE(c)    ((((c)<=0x7E)&&((c)>=0x20))?c:'.')
/*
 * Compile-time assertion helpers
 */
#define PASTE(a,b) a##b
#define ASSERT_LINE(predicate, line, file) \
    typedef char PASTE(assertion_failed_##file##_, line)[2*!!(predicate)-1];
/*
 * Compile-time assertion
 */
#define ASSERT(predicate, file) ASSERT_LINE(predicate, __LINE__, file)
/*
 * Memory management macros
 */
#define ALLOC(type, count) ((type*)calloc((count), sizeof(type)))
#define FREE(ptr) free((void*)ptr); ptr=NULL

#endif /* __MACRO_H__ */
