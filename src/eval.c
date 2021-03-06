#include "eval.h"

/* Namespace (all global) */
hashtable_t *funcDecs = NULL;
hashtable_t *varDecs = NULL;

jmp_buf endingJmpBuf;
jmp_buf continueJmpBuf;

ctx_table_t continueCtx;

int evaluate_condition(ifCondition_t *cond,
  EXPRESSION_PARAMS())
{
  /* Will set ax either 1 or 0 (or interrupt the program on error) */
  stackval_t svLeft;
  stackval_t svRight;

  /* Arbitrary double resoultion, not sure what to set this to */
  double epsilon = 0.00001;

  evaluate_expression(cond->left, EXPRESSION_ARGS());
  POP_VAL(&svLeft, sp, sc);

  evaluate_expression(cond->right, EXPRESSION_ARGS());
  POP_VAL(&svRight, sp, sc);

  switch (cond->type) 
  {
  case CONDITION_EQ:
  {
    if ( svLeft.type == INT32TYPE && svRight.type == INT32TYPE ) {

      if ( svLeft.i == svRight.i ) {
        *ax = 1;
      } else {
        *ax = 0;
      }

    } else if ( svLeft.type == INT32TYPE && svRight.type == DOUBLETYPE ) {
      *f0 = (double) svLeft.i;

      if ( fabs(*f0 - svRight.d) < epsilon ) {
        *ax = 1;
      } else {
        *ax = 0;
      }

    } else if ( svLeft.type == DOUBLETYPE && svRight.type == DOUBLETYPE ) {
      *f0 = svRight.d;
      *f1 = svLeft.d;

      if ( fabs(*f0 - *f1) < epsilon ) {
        *ax = 1;
      } else {
        *ax = 0;
      }
    } else if ( svLeft.type == DOUBLETYPE && svRight.type == INT32TYPE ) {
      *f0 = (double) svRight.i;

      if ( fabs(*f0 - svLeft.d) < epsilon ) {
        *ax = 1;
      } else {
        *ax = 0;
      }
    } else if ( svLeft.type == TEXT && svRight.type == TEXT ) {
      *ax = strcmp(svLeft.t, svRight.t) == 0;
    }
    break;
  }
  case CONDITION_NEQ:
  {
    if ( svLeft.type == INT32TYPE && svRight.type == INT32TYPE ) {

      if ( svLeft.i != svRight.i ) {
        *ax = 1;
      } else {
        *ax = 0;
      }

    } else if ( svLeft.type == INT32TYPE && svRight.type == DOUBLETYPE ) {
      *f0 = (double) svLeft.i;

      if ( fabs(*f0 - svRight.d) > epsilon ) {
        *ax = 1;
      } else {
        *ax = 0;
      }

    } else if ( svLeft.type == DOUBLETYPE && svRight.type == DOUBLETYPE ) {
      *f0 = svRight.d;
      *f1 = svLeft.d;

      if ( fabs(*f0 - *f1) > epsilon ) {
        *ax = 1;
      } else {
        *ax = 0;
      }
    } else if ( svLeft.type == DOUBLETYPE && svRight.type == INT32TYPE ) {
      *f0 = (double) svRight.i;

      if ( fabs(*f0 - svLeft.d) > epsilon ) {
        *ax = 1;
      } else {
        *ax = 0;
      }
    }
    break;
  }
  case CONDITION_LEQ:
  {
    if ( svLeft.type == INT32TYPE && svRight.type == INT32TYPE ) {

      if ( svLeft.i <= svRight.i ) {
        *ax = 1;
      } else {
        *ax = 0;
      }

    } else if ( svLeft.type == INT32TYPE && svRight.type == DOUBLETYPE ) {
      *f0 = (double) svLeft.i;

      if ( *f0 - svRight.d < -epsilon || fabs(*f0 - svRight.d) < epsilon ) {
        *ax = 1;
      } else {
        *ax = 0;
      }

    } else if ( svLeft.type == DOUBLETYPE && svRight.type == DOUBLETYPE ) {
      *f0 = svLeft.d;
      *f1 = svRight.d;

      if ( *f0 - *f1 < -epsilon || fabs(*f0 - *f1) < epsilon ) {
        *ax = 1;
      } else {
        *ax = 0;
      }
    } else if ( svLeft.type == DOUBLETYPE && svRight.type == INT32TYPE ) {
      *f1 = (double) svRight.i;

      if ( svLeft.d - *f1 < -epsilon || fabs(svLeft.d - *f1) < epsilon ) {
        *ax = 1;
      } else {
        *ax = 0;
      }
    }
    break;
  }
  case CONDITION_GEQ:
  {
    if ( svLeft.type == INT32TYPE && svRight.type == INT32TYPE ) {

      if ( svLeft.i >= svRight.i ) {
        *ax = 1;
      } else {
        *ax = 0;
      }

    } else if ( svLeft.type == INT32TYPE && svRight.type == DOUBLETYPE ) {
      *f0 = (double) svLeft.i;

      if ( *f0 - svRight.d > epsilon || fabs(*f0 - svRight.d) < epsilon ) {
        *ax = 1;
      } else {
        *ax = 0;
      }

    } else if ( svLeft.type == DOUBLETYPE && svRight.type == DOUBLETYPE ) {
      *f0 = svLeft.d;
      *f1 = svRight.d;

      if ( *f0 - *f1 > epsilon || fabs(*f0 - *f1) < epsilon ) {
        *ax = 1;
      } else {
        *ax = 0;
      }
    } else if ( svLeft.type == DOUBLETYPE && svRight.type == INT32TYPE ) {
      *f1 = (double) svRight.i;

      if ( svLeft.d - *f1 > epsilon || fabs(svLeft.d - *f1) < epsilon ) {
        *ax = 1;
      } else {
        *ax = 0;
      }
    }
    break;
  }
  case CONDITION_GE:
  {
    if ( svLeft.type == INT32TYPE && svRight.type == INT32TYPE ) {

      if ( svLeft.i > svRight.i ) {
        *ax = 1;
      } else {
        *ax = 0;
      }

    } else if ( svLeft.type == INT32TYPE && svRight.type == DOUBLETYPE ) {
      *f0 = (double) svLeft.i;

      if ( *f0 - svRight.d > epsilon ) {
        *ax = 1;
      } else {
        *ax = 0;
      }

    } else if ( svLeft.type == DOUBLETYPE && svRight.type == DOUBLETYPE ) {
      *f0 = svLeft.d;
      *f1 = svRight.d;

      if ( *f0 - *f1 > epsilon ) {
        *ax = 1;
      } else {
        *ax = 0;
      }
    } else if ( svLeft.type == DOUBLETYPE && svRight.type == INT32TYPE ) {
      *f1 = (double) svRight.i;

      if ( svLeft.d - *f1 > epsilon ) {
        *ax = 1;
      } else {
        *ax = 0;
      }
    }
    break;
  }
  case CONDITION_LE:
  {
    if ( svLeft.type == INT32TYPE && svRight.type == INT32TYPE ) {

      if ( svLeft.i < svRight.i ) {
        *ax = 1;
      } else {
        *ax = 0;
      }

    } else if ( svLeft.type == INT32TYPE && svRight.type == DOUBLETYPE ) {
      *f0 = (double) svLeft.i;

      if ( *f0 - svRight.d < -epsilon ) {
        *ax = 1;
      } else {
        *ax = 0;
      }

    } else if ( svLeft.type == DOUBLETYPE && svRight.type == DOUBLETYPE ) {
      *f0 = svLeft.d;
      *f1 = svRight.d;

      if ( *f0 - *f1 < -epsilon ) {
        *ax = 1;
      } else {
        *ax = 0;
      }
    } else if ( svLeft.type == DOUBLETYPE && svRight.type == INT32TYPE ) {
      *f1 = (double) svRight.i;

      if ( svLeft.d - *f1 < -epsilon ) {
        *ax = 1;
      } else {
        *ax = 0;
      }
    }
    break;
  }
  default:
  break;
  }

  return 0;
}

void free_vector(vector_t *vec) {
  /*argsList_t *walk = vec->content;

  while ( walk != NULL ) {
    if ( walk->arg != NULL ) {
      free_expression(walk->arg);
    }
    walk->arg = NULL;
    walk = walk->next;
  }*/
}

expr_t*  copy_vector(
  vector_t *vec,
  EXPRESSION_PARAMS()) {
  // The idea is to create a new vector based on
  // a raw one, where all identifiers have been replaced
  // with their actual value
  expr_t *newVec = newExpr_Vector(NULL);
  argsList_t *newContent = NULL;
  argsList_t *walk = vec->content;
  argsList_t *prev = NULL;
  argsList_t *current;
  argsList_t *walknext;

  while ( walk != NULL ) {
    stackval_t sv;
    expr_t *newExp;
    evaluate_expression(walk->arg, EXPRESSION_ARGS());
    POP_VAL(&sv, sp, sc);

    switch (sv.type) {
    case INT32TYPE:
      newExp = newExpr_Ival(sv.i);
      break;
    case DOUBLETYPE:
      newExp = newExpr_Float(sv.d);
      break;
    case TEXT:
      newExp = newExpr_Text(sv.t);
      break;
    case POINTERTYPE:
      newExp = newExpr_Pointer(sv.p);
      break;
    case FUNCPTRTYPE:
      newExp = newExpr_FuncPtr(sv.func);
      break;
    case LIBFUNCPTRTYPE:
      newExp = newExpr_LibFuncPtr(sv.libfunc);
      break;
    case VECTORTYPE: {
      newExp = copy_vector(sv.vec, EXPRESSION_ARGS());
      break;
    }
    default:
      printf("%s.error: unknown type of value on the stack (%d)\n", 
        __func__, sv.type);
      GENERAL_REPORT_ISSUE_MSG();
      break;
    }

    newContent = newArgument(newExp, newContent);
    newVec->vec->length++;
    newVec->vec->content = newContent;

    walk = walk->next;
  }

  /* Reverse the order of the vector expressions */
  /* Reverse the args list order */
  prev = NULL;
  current = newVec->vec->content;
  while (current != NULL) {
    walknext = current->next;
    current->next = prev;
    prev = current;
    current = walknext;
  }

  newVec->vec->content = prev;

  return newVec;
}


void evaluate_expression(
  expr_t *expr,
  EXPRESSION_PARAMS())
{
  if ( expr == NULL )
    return;

  switch (expr->type) 
  {
    case EXPR_TYPE_ID:
    {
      heapval_t *hv;
      int stop = 0;

      /* Check if this ID is among the arguments */
      argsList_t *walk = args;

      while ( walk != NULL && !stop ) {
        expr_t *exp = walk->arg;
        if ( exp->type == EXPR_TYPE_ID ) {
          expr_t *expArg;

          /* Check among the arguments if we have it defined there */
          expArg = hashtable_get(argVals, expr->id.id);

          if ( expArg != NULL ) {
            /* This was an argument! */
            switch ( expArg->type ) {
            case EXPR_TYPE_IVAL:
              PUSH_INT(expArg->ival, sp, sc);
              break;
            case EXPR_TYPE_POINTER:
              PUSH_POINTER(expArg->p, sp, sc);
              break;
            case EXPR_TYPE_VECTOR:
              PUSH_VECTOR(expArg->vec, sp, sc);
              break;
            case EXPR_TYPE_FVAL:
              PUSH_DOUBLE(expArg->fval, sp, sc);
              break;
            case EXPR_TYPE_FUNCPTR:
              PUSH_FUNCPTR(expArg->func, sp, sc);
              break;
            case EXPR_TYPE_LIBFUNCPTR:
              PUSH_LIBFUNCPTR(expArg->func, sp, sc);
              break;
            case EXPR_TYPE_DICT:
              PUSH_DICTIONARY(expArg->dict, sp, sc);
              break;
            case EXPR_TYPE_TEXT:
            {
              PUSH_STRING(expArg->text, sp, sc);
              break;
            }
            default:
              fprintf(stderr, "error: Invalid usage of identifier '%s'\n", expr->id.id);
              exit(1);
              break;
            }

            stop = 1;
          }
        } else {
          fprintf(stderr, "error: unknown, this is crazy. The interpreter is broken or something.\n\
Please report back to me.\n\
- %s\n", GENERAL_ERROR_ISSUE_URL);
          exit(1);
        }

        walk = walk->next;
      }

      if ( stop ) {
        break;
      }

      if ( walk == NULL ) {
        functionDef_t *funcDef; // if it is a function pointer
        /* Check among the variables if we have it defined there */
        hv = hashtable_get(varDecs, expr->id.id);
        if ( hv != NULL ) {

          switch ( hv->sv.type ) {
          case DOUBLETYPE:
            PUSH_DOUBLE(hv->sv.d, sp, sc);
            break;
          case POINTERTYPE:
            PUSH_POINTER(hv->sv.p, sp, sc);
            break;
          case VECTORTYPE:
            PUSH_VECTOR(hv->sv.vec, sp, sc);
            break;
          case INT32TYPE:
            PUSH_INT(hv->sv.i, sp, sc);
            break;
          case LIBFUNCPTRTYPE:
            PUSH_LIBFUNCPTR(hv->sv.libfunc, sp, sc);
            break;
          case FUNCPTRTYPE:
            PUSH_FUNCPTR(hv->sv.func, sp, sc);
            break;
          case DICTTYPE:
            PUSH_DICTIONARY(hv->sv.dict, sp, sc);
            break;
          case TEXT: {
            size_t len = strlen(hv->sv.t);
            stackval_t sv;
            heapval_t *hvp;
            int heapUpdated;

            char *newText = ast_emalloc(len+1);
            snprintf(newText, len+1, "%s", hv->sv.t);

            sv.type = TEXT;
            sv.t = newText;

            ALLOC_HEAP(&sv, hp, &hvp, &heapUpdated);

            if ( !heapUpdated ) {
              free(newText);
              sv = hvp->sv;
            }

            PUSH_STRING(sv.t, sp, sc);
            break;
          }
          default:
            break;
          }

          stop = 1;
        }

        if ( stop ) {
          break;
        }

        /* Check among the function declarations if we have it defined there */
        funcDef = hashtable_get(funcDecs, expr->id.id);
        if ( funcDef != NULL ) {
          // Pushing the function definition
          PUSH_FUNCPTR(funcDef, sp, sc);
          stop = 1;
        }

        if ( !stop ) {
           /* Check among the standard lib function declarations if we have it defined there */
          libFunction_t *libFunc = look_up_lib(expr->id.id);

          if ( libFunc != NULL ) {
            PUSH_LIBFUNCPTR(libFunc, sp, sc);
            stop = 1;
          } else {
            fprintf(stderr, "Failed to find ID: '%s'\n", expr->id.id);
            exit(1);
          }
        }

      }

      break;
    }
    case EXPR_TYPE_POINTER:
    PUSH_POINTER(expr->p, sp, sc);
    break;
    case EXPR_TYPE_FVAL:
    PUSH_DOUBLE(expr->fval, sp, sc);
    break;
    case EXPR_TYPE_FUNCPTR:
    PUSH_FUNCPTR(expr->func, sp, sc);
    break;
    case EXPR_TYPE_LIBFUNCPTR:
    PUSH_LIBFUNCPTR(expr->func, sp, sc);
    break;
    case EXPR_TYPE_VECTOR:
    PUSH_VECTOR(expr->vec, sp, sc);
    break;
    case EXPR_TYPE_DICT:
    PUSH_DICTIONARY(expr->dict, sp, sc);
    break;
    case EXPR_TYPE_VECTOR_IDX:
    {
      int32_t arrayIndex = 0;
      vector_t *vec;
      dictionary_t *dict;
      argsList_t *walk;
      int isDict = 0;
      expr_t *exp = NULL;
      expr_t *id = expr->vecIdx->id;
      expr_t *index = expr->vecIdx->index;

      stackval_t sv;

      if ( id->type == EXPR_TYPE_ID || id->type == EXPR_TYPE_VECTOR_IDX ) {
        evaluate_expression(id, EXPRESSION_ARGS());
        POP_VAL(&sv, sp, sc);

        if ( sv.type != VECTORTYPE && sv.type != DICTTYPE ) {
          fprintf(stderr, "index error: '%s' is a datatype the does not support indexing.\n", id->id.id);
          exit(1);
        }

        if ( sv.type == VECTORTYPE ) {
          vec = sv.vec;
          isDict = 0;
        } else if ( sv.type == DICTTYPE ) {
          isDict = 1;
          dict = sv.dict;
        }
      } else {
        fprintf(stderr, "error: Invalid indexing\n");
        exit(1);
      }

      evaluate_expression(index, EXPRESSION_ARGS());
      POP_VAL(&sv, sp, sc);

      if ( isDict == 0 ) {
        /* Array indexing */
        if ( sv.type != INT32TYPE ) {
          fprintf(stderr, "index error: Must provide an integer as index\n");
          exit(1);
        }

        arrayIndex = sv.i;

        /* check the limits */
        if ( arrayIndex >= vec->length ) {
          fprintf(stderr, "index error: index: '%" PRIi32 "' is too large, length: '%" PRIi32 "'\n",
            arrayIndex,
            vec->length);
          exit(1);
        }

        walk = vec->content;
        while ( walk != NULL && arrayIndex >= 0 ) {
          exp = walk->arg;
          walk = walk->next;
          --arrayIndex;
        }

        if ( exp == NULL ) {
          fprintf(stderr, "Unexpected index error!\n");
          fprintf(stderr, "Please include the script and file an error report to me here:\n    %s\n\
  This is not supposed to happen, I hope I can fix the intepreter!\n", GENERAL_ERROR_ISSUE_URL);
          exit(1);
        }

        /* Evaluate the expression */
        evaluate_expression(exp, EXPRESSION_ARGS());
        POP_VAL(&sv, sp, sc);

        /* Push value to the stack */
        switch (sv.type) {
          case INT32TYPE: {
            PUSH_INT(sv.i, sp, sc);
            break;
          }
          case DOUBLETYPE: {
            PUSH_DOUBLE(sv.d, sp, sc);
            break;
          }
          case TEXT: {
            PUSH_STRING(sv.t, sp, sc);
            break;
          }
          case POINTERTYPE: {
            PUSH_POINTER(sv.p, sp, sc);
            break;
          }
          case VECTORTYPE: {
            PUSH_VECTOR(sv.vec, sp, sc);
            break;
          }
          case FUNCPTRTYPE: {
            PUSH_FUNCPTR(sv.func, sp, sc);
            break;     
          }
          case LIBFUNCPTRTYPE: {
            PUSH_LIBFUNCPTR(sv.libfunc, sp, sc);
            break;
          }
          case DICTTYPE: {
            PUSH_DICTIONARY(sv.dict, sp, sc);
            break;
          }
          default:
            fprintf(stderr, "error: Unknown stackval_t type: %d\n", sv.type);
            exit(1);
            break;
        }

      } else {
        /* Dictionary indexing */
        char *key = NULL;
        heapval_t *hpv;

        if ( sv.type != TEXT ) {
          fprintf(stderr, "index error: Must provide an string as index for dictionaries\n");
          exit(1);
        }

        key = sv.t;

        /* find heapval */
        hpv = hashtable_get(dict->hash, key);
        if ( hpv == NULL ) {
          fprintf(stderr, "error: key '%s' not present in dictionary\n", key);
          exit(1);
        }

        sv = hpv->sv;

        /* Push value to the stack */
        switch (sv.type) {
          case INT32TYPE: {
            PUSH_INT(sv.i, sp, sc);
            break;
          }
          case DOUBLETYPE: {
            PUSH_DOUBLE(sv.d, sp, sc);
            break;
          }
          case TEXT: {
            PUSH_STRING(sv.t, sp, sc);
            break;
          }
          case POINTERTYPE: {
            PUSH_POINTER(sv.p, sp, sc);
            break;
          }
          case VECTORTYPE: {
            PUSH_VECTOR(sv.vec, sp, sc);
            break;
          }
          case FUNCPTRTYPE: {
            PUSH_FUNCPTR(sv.func, sp, sc);
            break;     
          }
          case LIBFUNCPTRTYPE: {
            PUSH_LIBFUNCPTR(sv.libfunc, sp, sc);
            break;
          }
          case DICTTYPE: {
            PUSH_DICTIONARY(sv.dict, sp, sc);
            break;
          }
          default:
            fprintf(stderr, "error: Unknown stackval_t type: %d\n", sv.type);
            exit(1);
            break;
        }

      }

    }
    break;
    case EXPR_TYPE_IVAL:
    PUSH_INT(expr->ival, sp, sc);
    break;
    case EXPR_TYPE_UVAL:
    PUSH_INT(expr->uval, sp, sc);
    break;
    case EXPR_TYPE_TEXT: {
      size_t len = strlen(expr->text);
      stackval_t sv;
      heapval_t *hvp;
      int heapUpdated;
      char *newText = ast_emalloc(len+1);
      snprintf(newText, len+1, "%s", expr->text);

      sv.type = TEXT;
      sv.t = newText;
  
      ALLOC_HEAP(&sv, hp, &hvp, &heapUpdated);

      if ( !heapUpdated ) {
        free(newText);
        sv = hvp->sv;
      }

      PUSH_STRING(sv.t, sp, sc);
      break;
    }
    break;
    case EXPR_TYPE_OPADD:
    {
      stackval_t svLeft;
      stackval_t svRight;

      evaluate_expression((expr_t*)expr->add.left, EXPRESSION_ARGS());
      POP_VAL(&svLeft, sp, sc);

      evaluate_expression((expr_t*)expr->add.right, EXPRESSION_ARGS());
      POP_VAL(&svRight, sp, sc);

      switch (svLeft.type) {
        case INT32TYPE: {
          *r0 = svLeft.i;
          break;
        }
        case DOUBLETYPE: {
          *f0 = svLeft.d;
          break;
        }
        case TEXT:
        case VECTORTYPE:
        case POINTERTYPE: 
          break;
        default:
          fprintf(stderr, "error: Unexpected stackval_t type: %d\n", svLeft.type);
          exit(1);
          break;
      }

      switch (svRight.type) {
        case INT32TYPE: {
          *r1 = svRight.i;
          break;
        }
        case DOUBLETYPE: {
          *f1 = svRight.d;
          break;
        }
        case TEXT:
        case VECTORTYPE:
        case POINTERTYPE: 
          break;
        default:
          fprintf(stderr, "error: Unexpected stackval_t type: %d\n", svRight.type);
          exit(1);
          break;
      }

      if ( svLeft.type == INT32TYPE && svRight.type == INT32TYPE ) {
        PUSH_INT(*r0+*r1, sp, sc);
      } else if ( svLeft.type == INT32TYPE && svRight.type == DOUBLETYPE ) {
        PUSH_DOUBLE(*r0 + *f1, sp, sc);
      } else if ( svLeft.type == DOUBLETYPE && svRight.type == DOUBLETYPE ) {
        PUSH_DOUBLE(*f0 + *f1, sp, sc);
      } else if ( svLeft.type == DOUBLETYPE && svRight.type == INT32TYPE ) {
        PUSH_DOUBLE(*f0 + *r1, sp, sc);
      } else if ( svLeft.type == TEXT && svRight.type == TEXT ) {
        size_t len = strlen(svLeft.t) + strlen(svRight.t);
        stackval_t sv;
        heapval_t *hvp;
        int heapUpdated;
        char *newText = ast_emalloc(len+1);
        snprintf(newText, len+1, "%s%s", svLeft.t, svRight.t);

        sv.type = TEXT;
        sv.t = newText;

        ALLOC_HEAP(&sv, hp, &hvp, &heapUpdated);

        if ( !heapUpdated ) {
          free(newText);
          sv = hvp->sv;
        }

        PUSH_STRING(sv.t, sp, sc);
      } else if ( svLeft.type == DOUBLETYPE && svRight.type == TEXT ) {
        size_t len = 50 + strlen(svRight.t);
        stackval_t sv;
        heapval_t *hvp;
        int heapUpdated;
        char *newText = ast_emalloc(len+1);
        snprintf(newText, len+1, "%.4f%s", svLeft.d, svRight.t);

        sv.type = TEXT;
        sv.t = newText;
  
        ALLOC_HEAP(&sv, hp, &hvp, &heapUpdated);

        if ( !heapUpdated ) {
          free(newText);
          sv = hvp->sv;
        }

        PUSH_STRING(sv.t, sp, sc);
      } else if ( svLeft.type == TEXT && svRight.type == DOUBLETYPE ) {
        size_t len = 50 + strlen(svLeft.t);
        stackval_t sv;
        heapval_t *hvp;
        int heapUpdated;
        char *newText = ast_emalloc(len+1);
        snprintf(newText, len+1, "%s%.4f", svLeft.t, svRight.d);

        sv.type = TEXT;
        sv.t = newText;
  
        ALLOC_HEAP(&sv, hp, &hvp, &heapUpdated);

        if ( !heapUpdated ) {
          free(newText);
          sv = hvp->sv;
        }

        PUSH_STRING(sv.t, sp, sc);
      } else if ( svLeft.type == TEXT && svRight.type == INT32TYPE ) {
        size_t len = 50 + strlen(svLeft.t);
        stackval_t sv;
        heapval_t *hvp;
        int heapUpdated;
        char *newText = ast_emalloc(len+1);
        snprintf(newText, len+1, "%s%d", svLeft.t, svRight.i);

        sv.type = TEXT;
        sv.t = newText;

        ALLOC_HEAP(&sv, hp, &hvp, &heapUpdated);

        if ( !heapUpdated ) {
          free(newText);
          sv = hvp->sv;
        }

        PUSH_STRING(sv.t, sp, sc);
      } else if ( svLeft.type == INT32TYPE && svRight.type == TEXT ) {
        size_t len = 50 + strlen(svRight.t);
        stackval_t sv;
        heapval_t *hvp;
        int heapUpdated;
        char *newText = ast_emalloc(len+1);
        snprintf(newText, len+1, "%d%s", svLeft.i, svRight.t);

        sv.type = TEXT;
        sv.t = newText;
  
        ALLOC_HEAP(&sv, hp, &hvp, &heapUpdated);

        if ( !heapUpdated ) {
          free(newText);
          sv = hvp->sv;
        }

        PUSH_STRING(sv.t, sp, sc);
      } else if ( svLeft.type == POINTERTYPE && svRight.type == TEXT ) {
        size_t len = 50 + strlen(svRight.t);
        stackval_t sv;
        heapval_t *hvp;
        int heapUpdated;
        char *newText = ast_emalloc(len+1);
        snprintf(newText, len+1, "<%" PRIuPTR ">%s", svLeft.p, svRight.t);

        sv.type = TEXT;
        sv.t = newText;
  
        ALLOC_HEAP(&sv, hp, &hvp, &heapUpdated);

        if ( !heapUpdated ) {
          free(newText);
          sv = hvp->sv;
        }

        PUSH_STRING(sv.t, sp, sc);
      } else if ( svLeft.type == TEXT && svRight.type == POINTERTYPE ) {
        size_t len = 50 + strlen(svLeft.t);
        stackval_t sv;
        heapval_t *hvp;
        int heapUpdated;
        char *newText = ast_emalloc(len+1);
        snprintf(newText, len+1, "%s<%" PRIuPTR ">", svLeft.t, svRight.p);

        sv.type = TEXT;
        sv.t = newText;
  
        ALLOC_HEAP(&sv, hp, &hvp, &heapUpdated);

        if ( !heapUpdated ) {
          free(newText);
          sv = hvp->sv;
        }

        PUSH_STRING(sv.t, sp, sc);
      } else if ( svLeft.type == VECTORTYPE && svRight.type == TEXT ) {
        size_t len = 50 + strlen(svRight.t);
        stackval_t sv;
        heapval_t *hvp;
        int heapUpdated;
        char *newText = ast_emalloc(len+1);
        snprintf(newText, len+1, "<List:size=%" PRIi32 ">%s", svLeft.vec->length, svRight.t);

        sv.type = TEXT;
        sv.t = newText;
  
        ALLOC_HEAP(&sv, hp, &hvp, &heapUpdated);

        if ( !heapUpdated ) {
          free(newText);
          sv = hvp->sv;
        }

        PUSH_STRING(sv.t, sp, sc);
      } else if ( svLeft.type == TEXT && svRight.type == VECTORTYPE ) {
        size_t len = 50 + strlen(svRight.t);
        stackval_t sv;
        heapval_t *hvp;
        int heapUpdated;
        char *newText = ast_emalloc(len+1);
        snprintf(newText, len+1, "%s<List:size=%" PRIi32 ">", svLeft.t, svRight.vec->length);

        sv.type = TEXT;
        sv.t = newText;
  
        ALLOC_HEAP(&sv, hp, &hvp, &heapUpdated);

        if ( !heapUpdated ) {
          free(newText);
          sv = hvp->sv;
        }

        PUSH_STRING(sv.t, sp, sc);
      }

      break;
    }
    break;
    case EXPR_TYPE_OPSUB:
    {
      stackval_t svLeft;
      stackval_t svRight;

      evaluate_expression((expr_t*)expr->add.left, EXPRESSION_ARGS());
      POP_VAL(&svLeft, sp, sc);

      evaluate_expression((expr_t*)expr->add.right, EXPRESSION_ARGS());
      POP_VAL(&svRight, sp, sc);

      switch (svLeft.type) {
        case INT32TYPE: {
          *r0 = svLeft.i;
          break;
        }
        case DOUBLETYPE: {
          *f0 = svLeft.d;
          break;
        }
        case TEXT: {
          fprintf(stderr, "error: Cannot substract strings..\n");
          exit(1);
          break;
        }
        default:
          fprintf(stderr, "error: Unexpected stackval_t type: %d\n", svLeft.type);
          exit(1);
          break;
      }

      switch (svRight.type) {
        case INT32TYPE: {
          *r1 = svRight.i;
          break;
        }
        case DOUBLETYPE: {
          *f1 = svRight.d;
          break;
        }
        case TEXT: {
          fprintf(stderr, "error: Cannot substract strings..\n");
          exit(1);
          break;
        }
        default:
          fprintf(stderr, "error: Unexpected stackval_t type: %d\n", svRight.type);
          exit(1);
          break;
      }

      if ( svLeft.type == INT32TYPE && svRight.type == INT32TYPE ) {
        PUSH_INT(*r0-*r1, sp, sc);
      } else if ( svLeft.type == INT32TYPE && svRight.type == DOUBLETYPE ) {
        PUSH_DOUBLE(*r0 - *f1, sp, sc);
      } else if ( svLeft.type == DOUBLETYPE && svRight.type == DOUBLETYPE ) {
        PUSH_DOUBLE(*f0 - *f1, sp, sc);
      } else if ( svLeft.type == DOUBLETYPE && svRight.type == INT32TYPE ) {
        PUSH_DOUBLE(*f0 - *r1, sp, sc);
      }

      break;
    }
    case EXPR_TYPE_OPMUL:
    {
      stackval_t svLeft;
      stackval_t svRight;

      evaluate_expression((expr_t*)expr->add.left, EXPRESSION_ARGS());
      POP_VAL(&svLeft, sp, sc);

      evaluate_expression((expr_t*)expr->add.right, EXPRESSION_ARGS());
      POP_VAL(&svRight, sp, sc);

      switch (svLeft.type) {
        case INT32TYPE: {
          *r0 = svLeft.i;
          break;
        }
        case DOUBLETYPE: {
          *f0 = svLeft.i;
          break;
        }
        case TEXT: {
          fprintf(stderr, "error: Cannot multiply strings..\n");
          exit(1);
          break;
        }
        default:
          fprintf(stderr, "error: Unexpected stackval_t type: %d\n", svLeft.type);
          exit(1);
          break;
      }

      switch (svRight.type) {
        case INT32TYPE: {
          *r1 = svRight.i;
          break;
        }
        case DOUBLETYPE: {
          *f1 = svRight.i;
          break;
        }
        case TEXT: {
          fprintf(stderr, "error: Cannot multiply strings..\n");
          exit(1);
          break;
        }
        default:
          fprintf(stderr, "error: Unexpected stackval_t type: %d\n", svRight.type);
          exit(1);
          break;
      }

      if ( svLeft.type == INT32TYPE && svRight.type == INT32TYPE ) {
        PUSH_INT(*r0 * *r1, sp, sc);
      } else if ( svLeft.type == INT32TYPE && svRight.type == DOUBLETYPE ) {
        PUSH_DOUBLE(*r0 * *f1, sp, sc);
      } else if ( svLeft.type == DOUBLETYPE && svRight.type == DOUBLETYPE ) {
        PUSH_DOUBLE(*f0 * *f1, sp, sc);
      } else if ( svLeft.type == DOUBLETYPE && svRight.type == INT32TYPE ) {
        PUSH_DOUBLE(*f0 * *r1, sp, sc);
      }

      break;
    }
    case EXPR_TYPE_OPMOD:
    {
      stackval_t svLeft;
      stackval_t svRight;

      evaluate_expression((expr_t*)expr->add.left, EXPRESSION_ARGS());
      POP_VAL(&svLeft, sp, sc);

      evaluate_expression((expr_t*)expr->add.right, EXPRESSION_ARGS());
      POP_VAL(&svRight, sp, sc);

      switch (svLeft.type) {
        case INT32TYPE: {
          *r0 = svLeft.i;
          break;
        }
        case DOUBLETYPE: {
          fprintf(stderr, "error: Invalid expression, cannot calculate modulus on floating point.\n");
          exit(1);
          break;
        }
        case TEXT: {
          fprintf(stderr, "error: Invalid expression, cannot calculate modulus on string.\n");
          exit(1);
          break;
        }
        default:
          fprintf(stderr, "error: Unexpected stackval_t type: %d\n", svLeft.type);
          exit(1);
          break;
      }

      switch (svRight.type) {
        case INT32TYPE: {
          *r1 = svRight.i;
          break;
        }
        case DOUBLETYPE: {
          fprintf(stderr, "error: Invalid expression, cannot calculate modulus on floating point.\n");
          exit(1);
          break;
        }
        case TEXT: {
          fprintf(stderr, "error: Invalid expression, cannot calculate modulus on string.\n");
          exit(1);
          break;
        }
        default:
          fprintf(stderr, "error: Unexpected stackval_t type: %d\n", svRight.type);
          exit(1);
          break;
      }

      if ( svLeft.type == INT32TYPE && svRight.type == INT32TYPE ) {
        PUSH_INT(*r0 % *r1, sp, sc);
      }

      break;
    }
    break;
    case EXPR_TYPE_OPDIV:
    {
      stackval_t svLeft;
      stackval_t svRight;

      evaluate_expression((expr_t*)expr->add.left, EXPRESSION_ARGS());
      POP_VAL(&svLeft, sp, sc);

      evaluate_expression((expr_t*)expr->add.right, EXPRESSION_ARGS());
      POP_VAL(&svRight, sp, sc);

      switch (svLeft.type) {
        case INT32TYPE: {
          *r0 = svLeft.i;
          break;
        }
        case DOUBLETYPE: {
          *f0 = svLeft.i;
          break;
        }
        case TEXT: {
          fprintf(stderr, "error: Not implemented string additions yet..\n");
          exit(1);
          break;
        }
        default:
          fprintf(stderr, "error: Unexpected stackval_t type: %d\n", svLeft.type);
          exit(1);
          break;
      }

      switch (svRight.type) {
        case INT32TYPE: {
          *r1 = svRight.i;
          break;
        }
        case DOUBLETYPE: {
          *f1 = svRight.i;
          break;
        }
        case TEXT: {
          fprintf(stderr, "error: Not implemented string additions yet..\n");
          exit(1);
          break;
        }
        default:
          fprintf(stderr, "error: Unexpected stackval_t type: %d\n", svRight.type);
          exit(1);
          break;
      }

      if ( svLeft.type == INT32TYPE && svRight.type == INT32TYPE ) {
        PUSH_INT(*r0 / *r1, sp, sc);
      } else if ( svLeft.type == INT32TYPE && svRight.type == DOUBLETYPE ) {
        PUSH_DOUBLE(*r0 / *f1, sp, sc);
      } else if ( svLeft.type == DOUBLETYPE && svRight.type == DOUBLETYPE ) {
        PUSH_DOUBLE(*f0 / *f1, sp, sc);
      } else if ( svLeft.type == DOUBLETYPE && svRight.type == INT32TYPE ) {
        PUSH_DOUBLE(*f0 / *r1, sp, sc);
      }

      break;
    }
    break;
    case EXPR_TYPE_COND:
    {
      evaluate_condition(expr->cond, stmt, next, PROVIDE_CONTEXT(), args, argVals);
      /* Push ax to stack */
      PUSH_INT(*ax, sp, sc);
      break;
    }
    case EXPR_TYPE_FUNCCALL:
    {
      stackval_t sv;
      functionCall_t *func = (functionCall_t *) expr->func;

      call_func(func, stmt, next, PROVIDE_CONTEXT(), args, argVals);
      POP_VAL(&sv, sp, sc);

      /* Push value to the stack */
      switch (sv.type) {
        case INT32TYPE: {
          PUSH_INT(sv.i, sp, sc);
          break;
        }
        case DOUBLETYPE: {
          PUSH_DOUBLE(sv.d, sp, sc);
          break;
        }
        case TEXT: {
          PUSH_STRING(sv.t, sp, sc);
          break;
        }
        case POINTERTYPE: {
          PUSH_POINTER(sv.p, sp, sc);
          break;
        }
        case VECTORTYPE: {
          PUSH_VECTOR(sv.vec, sp, sc);
          break;
        }
        case FUNCPTRTYPE: {
          PUSH_FUNCPTR(sv.func, sp, sc);
          break;     
        }
        case LIBFUNCPTRTYPE: {
          PUSH_LIBFUNCPTR(sv.libfunc, sp, sc);
          break;
        }
        case DICTTYPE: {
          PUSH_DICTIONARY(sv.dict, sp, sc);
          break;
        }
        default:
          fprintf(stderr, "error: Unknown stackval_t type: %d\n", sv.type);
          exit(1);
          break;
      }
      break;
    }
    case EXPR_TYPE_EMPTY:
    default:
    break;
  }
}

void call_func(
  functionCall_t *funcCall,
  EXPRESSION_PARAMS())
{
  functionDef_t *funcDef = NULL;
  argsList_t *argsWalk = funcCall->args;
  hashtable_t *newArgumentTable = new_argstable();
  stackval_t sv;
  stackval_t sv_ret;
  expr_t *expArg;
  libFunction_t *libFunc = NULL;
  char *funcID;

  /* Evaluate id */
  evaluate_expression(funcCall->id, EXPRESSION_ARGS());
  POP_VAL(&sv, sp, sc);

  switch ( sv.type ) {
  case TEXT:
    funcID = sv.t;
    break;
  case FUNCPTRTYPE: {
    functionDef_t *func = sv.func;
    funcID = func->id.id;
    break;
  }
  case LIBFUNCPTRTYPE: {
    libFunction_t *func = sv.libfunc;
    funcID = func->libFuncName;
    break;
  }
  default:
    fprintf(stderr, "error: invalid function call\n");
    exit(1);
    break;
  }

  /* Check among the arguments if we have it defined there */
  expArg = hashtable_get(argVals, funcID);

  /* The argument might be a function! Evaluate and see ... */
  if ( expArg != NULL ) {
    /* This was an argument! */
    switch ( expArg->type ) {
    case EXPR_TYPE_FUNCPTR:
      funcDef = expArg->func;
      break;
    case EXPR_TYPE_LIBFUNCPTR:
      libFunc = expArg->func;
      break;
    default:
      fprintf(stderr, "error: Invalid usage of identifier '%s'\n", expArg->id.id);
      exit(1);
      break;
    }

  }

  if ( funcDef == NULL && libFunc == NULL ) {

    /* Looking up the function and calling it if it exists */
    funcDef = hashtable_get(funcDecs, funcID);
    /* Looking up the function among the library */
    libFunc = look_up_lib(funcID);

    /* Check lookup status */
    if ( funcDef == NULL && libFunc == NULL ) {
      heapval_t *hv;
      /* Check if this is a function pointer call (lowest priority) */
      hv = hashtable_get(varDecs, funcID);

      if ( hv == NULL ) {
        // Check among the arguments 

        /* Check among the arguments if we have it defined there */
        expArg = hashtable_get(argVals, funcID);

        if ( expArg == NULL ) {
          fprintf(stderr, "Error: Function call undefined: '%s'.\r\n", funcID);
          exit(1);
        }

        /* The argument might be a function! Evaluate and see ... */
        if ( expArg != NULL ) {
          /* This was an argument! */
          switch ( expArg->type ) {
          case EXPR_TYPE_FUNCCALL:
            funcDef = expArg->func;
            break;
          case EXPR_TYPE_LIBFUNCPTR:
            libFunc = expArg->func;
            break;
          default:
            fprintf(stderr, "error: Invalid usage of identifier '%s'\n", expArg->id.id);
            exit(1);
            break;
          }

        }

      } else if ( hv->sv.type != FUNCPTRTYPE && hv->sv.type != LIBFUNCPTRTYPE ) {
        fprintf(stderr, "Error: Function call undefined: '%s'.\r\n", funcID);
        exit(1);
      } else {
        switch (hv->sv.type) {
        case FUNCPTRTYPE:
          funcDef = hv->sv.func;
          break;
        case LIBFUNCPTRTYPE:
          libFunc = hv->sv.libfunc;
          break;
          default:
          // This is just not supposed to be possible, look above.
          break;
        }
      }

    }
  }

  if ( funcDef != NULL ) {
    uintptr_t spBefore;
    int stackAllocSpEd = 0;

    /* Check that # parameters == # arguments */
    argsList_t *params = funcDef->params;

    if ( params == NULL && argsWalk != NULL ) {
      fprintf(stderr, "Error: function '%s' expected 0 arguments, got: %u\n",
        funcID, argsWalk->length );
      exit(1);
    }

    if ( params != NULL && argsWalk == NULL ) {
      fprintf(stderr, "Error: function '%s' expected %u arguments, got: 0\n",
        funcID, params->length );
      exit(1);
    }

    if ( params != NULL && argsWalk != NULL )  {
      /* Verifying function definition parameters and function call arguments */
      if ( params->length != argsWalk->length ) {
        /* print error message */
        fprintf(stderr, "Error: function '%s' expected %u arguments, got: %u\n",
          funcID, params->length, argsWalk->length);
        exit(1);
      }

      /* Populate arguments */
      while ( argsWalk != NULL && params != NULL ) {
        stackval_t sv;
        expr_t *newArg = NULL;

        if ( params->arg->type != EXPR_TYPE_ID ) {
          /* This is not supposed to happen */
          printf("Error: parameter in function definition '%s' was invalid\n",
            funcID);
        }

        /* Evaluate expression */
        evaluate_expression(argsWalk->arg, EXPRESSION_ARGS());
        
        /* Fetch the evaluated expression to the arguments table */
        POP_VAL(&sv, sp, sc);

        switch (sv.type) {
          case INT32TYPE: {
            newArg = newExpr_Ival(sv.i);
            break;
          }
          case DOUBLETYPE: {
            newArg = newExpr_Float(sv.d);
            break;
          }
          case TEXT: {
            newArg = newExpr_Text(sv.t);
            break;
          }
          case POINTERTYPE: {
            newArg = newExpr_Pointer(sv.p);
            break;
          }
          case VECTORTYPE: {
            newArg = newExpr_Vector(sv.vec->content);
            break;
          }
          case FUNCPTRTYPE: {
            newArg = newExpr_FuncPtr(sv.func);
            break;
          }
          case LIBFUNCPTRTYPE: {
            newArg = newExpr_LibFuncPtr(sv.libfunc);
            break;
          }
          case DICTTYPE: {
            expr_t *e = ast_emalloc(sizeof(expr_t));
            e->type = EXPR_TYPE_DICT;
            e->dict = allocNewDictionary(sv.dict, EXPRESSION_ARGS());
            newArg = e;
            break;
          }
          default:
            fprintf(stderr, "error: sorry but argument datatype cannot be passed to a function, type: %d\n", sv.type);
            exit(1);
            break;
        }

        /* Adding expression to argument table */
        hashtable_put(newArgumentTable, params->arg->id.id, newArg);

        params = params->next;
        argsWalk = argsWalk->next;
      }

    }

    /* Default return type: a zero */
    sv_ret.type = INT32TYPE;
    sv_ret.i = 0;

    if ( (*(uintptr_t*)st)  != (uintptr_t) stmt ) {

      PUSH_POINTER((*(uintptr_t*)st), sp, sc);
      (*(uintptr_t*)st) = (uintptr_t) stmt;
      PUSH_POINTER((*(uintptr_t*)ed), sp, sc);
      (*(uintptr_t*)ed) = (uintptr_t) next;

      stackAllocSpEd = 1;
    }

    spBefore = *(uintptr_t*)sp;

    /* Call the function */
    if ( funcDecs ) {
      /* Moving along, interpreting function*/
      interpret_statements_(funcDef->body, PROVIDE_CONTEXT(), funcDef->params, newArgumentTable);
    }

    if ( *(uintptr_t*)sp != spBefore ) {
      /* No return statement found, pushing 0 on the stack */
      POP_VAL(&sv_ret, sp, sc);
    }

    if ( stackAllocSpEd ) {
      /* Resetting original values */
      POP_VAL(&sv, sp, sc);
      (*(uintptr_t*)ed) = sv.p;
      POP_VAL(&sv, sp, sc);
      (*(uintptr_t*)st) = sv.p;
    }

    /* Push function return value to the stack */
    switch (sv_ret.type) {
      case INT32TYPE: {
        PUSH_INT(sv_ret.i, sp, sc);
        break;
      }
      case DOUBLETYPE: {
        PUSH_DOUBLE(sv_ret.d, sp, sc);
        break;
      }
      case TEXT: {
        PUSH_STRING(sv_ret.t, sp, sc);
        break;
      }
      case POINTERTYPE: {
        PUSH_POINTER(sv_ret.p, sp, sc);
        break;
      }
      case VECTORTYPE: {
        PUSH_VECTOR(sv_ret.vec, sp, sc);
        break;
      }
      case DICTTYPE: {
        PUSH_DICTIONARY(sv_ret.dict, sp, sc);
        break;
      }
      default:
        fprintf(stderr, "error: Unknown stackval_t type: %d\n", sv.type);
        exit(1);
        break;
    }

    /* Free the argument value table */
    flush_arguments(newArgumentTable);

  } else {
    /* This is a library function */
    int libfunc_ret;

    if ( libFunc->nbrArgs > 0 && argsWalk == NULL ) {
      fprintf(stderr, "error: library function '%s' need %d agument%s, %d provided.\n",
        funcID, libFunc->nbrArgs, (libFunc->nbrArgs == 1 ? "" : "s"), 0);
      exit(1);
    }

    if ( libFunc->nbrArgs != (int)argsWalk->length ) {
      fprintf(stderr, "error: library function '%s' need %d agument%s, %d provided.\n",
        funcID, libFunc->nbrArgs, (libFunc->nbrArgs == 1 ? "" : "s"), (int)argsWalk->length);
      exit(1);
    }

    /* Populate arguments */
    while ( argsWalk != NULL ) {
      stackval_t sv;

      /* Evaluate expression */
      evaluate_expression(argsWalk->arg, EXPRESSION_ARGS());
      
      /* Fetch the evaluated expression to the arguments table */
      POP_VAL(&sv, sp, sc);

      switch (sv.type) {
        case INT32TYPE: 
        {
          PUSH_INT(sv.i, sp, sc);
          break;
        }
        case DOUBLETYPE:
        {
          PUSH_DOUBLE(sv.d, sp, sc);
          break;
        }
        case TEXT:
        {
          PUSH_STRING(sv.t, sp, sc);
          break;
        }
        case POINTERTYPE: {
          PUSH_POINTER(sv.p, sp, sc);
          break;
        }
        case VECTORTYPE: {
          PUSH_VECTOR(sv.vec, sp, sc);
          break;
        }
        case DICTTYPE: {
          PUSH_DICTIONARY(sv.dict, sp, sc);
          break;
        }
        default:
        {
          fprintf(stderr, "error: Unknown stackval_t type: %d\n", sv.type);
          exit(1);
          break;
        }
      }

      argsWalk = argsWalk->next;
    }

    libfunc_ret = libFunc->func(funcID, EXPRESSION_ARGS() );

    /* Free the argument value table */
    flush_arguments(newArgumentTable);

    if ( libfunc_ret != 0 ) {
      fprintf(stderr, "Error during execution of library function '%s', error code: %d\n",
        funcID, libfunc_ret);
      exit(libfunc_ret);
    }
  }

}

void interpret_statements_(
  void *stmt,
  PROVIDE_CONTEXT_ARGS(),
  argsList_t *args,
  hashtable_t *argVals
)
{
  static int premier = 1;
  entity_eval_t *eval;
  void *next = NULL;

  if ( premier ) {
    switch ( setjmp(continueJmpBuf) ) {
      case JMP_CODE_CONTINUE: {
        stmt = continueCtx.stmt;
        args = continueCtx.args;
        argVals = continueCtx.argVals;
      }
      break;
      case JMP_CODE_INITIAL:
      /* Fall through */
      default:
      /* It is OK, just continue */
      break;
    }
    premier = 0;
  }

  if ( stmt == NULL )
    return;

  eval = (entity_eval_t*)stmt;

  switch ( eval->entity ) {
    case LANG_ENTITY_DECL:
    case LANG_ENTITY_FUNCDECL:
    case LANG_ENTITY_FUNCCALL:
    case LANG_ENTITY_CONDITIONAL:
    case LANG_ENTITY_SYSTEM:
      next = ((statement_t*)stmt)->next;
    break;
    case LANG_ENTITY_RETURN:
      next = NULL;
    break;
    case LANG_ENTITY_EMPTY_MATH:
    case LANG_ENTITY_EMPTY_STR:
    { 
      stackval_t sv;
      expr_t *e = ((statement_t*)stmt)->content;
      evaluate_expression(e, EXPRESSION_ARGS());
      POP_VAL(&sv, sp, sc);
      switch ( sv.type ) {
      case INT32TYPE:
      printf("%" PRIi32 "\n", sv.i);
      break;
      case DOUBLETYPE:
      printf("%lf\n", sv.d);
      break;
      case TEXT:
      printf("%s\n", sv.t);
      break;
      default:
      printf("%s.error: unknown type of value on the stack (%d)\n", 
        __func__, sv.type);
      break;
      }

      next = ((statement_t*)stmt)->next;
      break;
    }
    case LANG_ENTITY_BODY: {
      next = ((body_t*)stmt)->content;
    }
    break;
    case LANG_ENTITY_FIN: {
      /* Jump to VM shutdown */
      longjmp(endingJmpBuf, 1);
    }
    break;
    default:
    break;
  }

  switch ( eval->entity ) {
    case LANG_ENTITY_DECL:
    {
      stackval_t sv;
      expr_t *id;
      heapval_t *hvp = NULL;
      declaration_t* decl = ((statement_t*)stmt)->content;
      id = decl->id;

      switch ( id->type ) {
      case EXPR_TYPE_ID: {
        int heapUpdated;
        char *idStr = id->id.id;

        /* Evaluating the expression among global variables */
        evaluate_expression(decl->val, EXPRESSION_ARGS());
        POP_VAL(&sv, sp, sc);

        /* Placing value on the heap */
        if ( sv.type == TEXT ) {
          /* Special case */
          char *c = sv.t;
          size_t len = strlen(c)+1;
          char *newText = ast_emalloc(len);
          snprintf(newText,len,"%s",c);
          sv.t = newText;
        } else if ( sv.type == VECTORTYPE ) {
          expr_t *e = copy_vector(sv.vec, EXPRESSION_ARGS());
          sv.vec = e->vec;
          free(e);
        } else if ( sv.type == DICTTYPE ) {
          dictionary_t *dict = allocNewDictionary(sv.dict, EXPRESSION_ARGS());
          sv.dict = dict;
        }

        ALLOC_HEAP(&sv, hp, &hvp, &heapUpdated);

        /* Placing variable declaration in global variable namespace */
        hashtable_put(varDecs, idStr, hvp);
      }
      break;
      case EXPR_TYPE_VECTOR_IDX: {
        vector_t *vec = NULL;
        dictionary_t *dict = NULL;
        int32_t arrayIndex;
        argsList_t *walk;
        int isDict = 0;
        expr_t **expToSet = NULL;
        expr_t *vecid = id->vecIdx->id;
        expr_t *index = id->vecIdx->index;

        stackval_t sv;

        evaluate_expression(vecid, EXPRESSION_ARGS());
        POP_VAL(&sv, sp, sc);

        if ( sv.type != VECTORTYPE && sv.type != DICTTYPE ) {
          fprintf(stderr, "index error: '%s' is not an indexable object.\n", id->id.id);
          GENERAL_REPORT_ISSUE_MSG();
          exit(1);
        }

        if ( sv.type == DICTTYPE ) {
          isDict = 1;
          dict = sv.dict;
        } else {
          isDict = 0;
        }

        if ( isDict == 0 ) {
          /* Assigning a vector */
          vec = sv.vec;

          evaluate_expression(index, EXPRESSION_ARGS());
          POP_VAL(&sv, sp, sc);

          if ( sv.type != INT32TYPE ) {
            fprintf(stderr, "index error: Must provide an integer as index\n");
            exit(1);
          }

          arrayIndex = sv.i;

          /* check the limits */
          if ( arrayIndex >= vec->length ) {
            fprintf(stderr, "index error: index: '%" PRIi32 "' is too large, length: '%" PRIi32 "'\n",
              arrayIndex,
              vec->length);
            exit(1);
          }

          walk = vec->content;
          while ( walk != NULL && arrayIndex >= 0 ) {
            expToSet = &walk->arg;
            walk = walk->next;
            --arrayIndex;
          }

          if ( *expToSet == NULL ) {
            fprintf(stderr, "Unexpected index error!\n");
            GENERAL_REPORT_ISSUE_MSG();
            exit(1);
          }

          /* Placing this expression into the array */
          free_expression(*expToSet);
          free(*expToSet);
          *expToSet = decl->val;
        } else {
          /* Assigning a dictionary */
          char *key = NULL;
          heapval_t *hvp = NULL;
          int dummy;

          evaluate_expression(index, EXPRESSION_ARGS());
          POP_VAL(&sv, sp, sc);

          if ( sv.type != TEXT ) {
            fprintf(stderr, "index error: Must provide a string as key\n");
            exit(1);
          }

          key = ast_emalloc(strlen(sv.t)+2);
          snprintf(key, strlen(sv.t)+1, "%s", sv.t);

          /* Evaluating the expression among global variables */
          evaluate_expression(decl->val, EXPRESSION_ARGS());
          POP_VAL(&sv, sp, sc);

          /* Placing value on the heap */
          if ( sv.type == TEXT ) {
            /* Special case */
            char *c = sv.t;
            size_t len = strlen(c)+1;
            char *newText = ast_emalloc(len);
            snprintf(newText,len,"%s",c);
            sv.t = newText;
          } else if ( sv.type == VECTORTYPE ) {
            expr_t *e = copy_vector(sv.vec, EXPRESSION_ARGS());
            sv.vec = e->vec;
            free(e);
          } else if ( sv.type == DICTTYPE ) {
            dictionary_t *dict = allocNewDictionary(sv.dict, EXPRESSION_ARGS());
            sv.dict = dict;
          }

          ALLOC_HEAP(&sv, hp, &hvp, &dummy);

          // Check if collision, if so, free key
          hashtable_put(dict->hash, key, hvp);
        }
      }
      break;
      default:
        GENERAL_REPORT_ISSUE_MSG();
        break;
      }

      // Mark and sweep the heap
      (void)set_mark_value();
      mark_and_sweep(varDecs, EXPRESSION_ARGS());
    }
    break;
    case LANG_ENTITY_SYSTEM:
    {
      expr_t *sys_var = (expr_t*)((statement_t*)stmt)->content;
      stackval_t sv;

      evaluate_expression(sys_var, EXPRESSION_ARGS());
      POP_VAL(&sv, sp, sc);
      switch ( sv.type ) {
      case TEXT:
        /* Making the system call */
        system(sv.t);
        break;
      default:
        printf("%s.error: unknown type of system call on the stack (%d)\n", 
          __func__, sv.type);
        break;
      }
    }
    break;
    case LANG_ENTITY_RETURN:
    {
      expr_t *retVal = (expr_t*)((statement_t*)stmt)->content;
      stackval_t sv;

      evaluate_expression(retVal, EXPRESSION_ARGS());
      POP_VAL(&sv, sp, sc);
      switch ( sv.type ) {
      case TEXT:
        /* Pushing the return value as a string */
        PUSH_STRING(sv.t, sp, sc);
        break;
      case INT32TYPE:
        /* Pushing the return value as an int */
        PUSH_INT(sv.i, sp, sc);
        break;
      case VECTORTYPE:
        /* Pushing the return value as a vector */
        PUSH_VECTOR(sv.vec, sp, sc);
        break;
      case DOUBLETYPE:
        /* Pushing the return value as a double */
        PUSH_DOUBLE(sv.d, sp, sc);
      break;
      default:
        printf("%s.error: unknown type of system call on the stack (%d)\n", 
          __func__, sv.type);
        break;
      }
    }
    break;
    case LANG_ENTITY_FUNCDECL:
    {
      functionDef_t *funcDef = ((statement_t*)stmt)->content;

      /* Placing funciton declaration in global function namespace */
      hashtable_put(funcDecs, funcDef->id.id, funcDef);
    }
    break;
    case LANG_ENTITY_CONTINUE:
    {
      /* Set PC to continue 'st' */
      continueCtx.stmt = *st;
      continueCtx.args = args;
      continueCtx.argVals = argVals;
      longjmp(continueJmpBuf, JMP_CODE_CONTINUE);
    }
    break;
    case LANG_ENTITY_BREAK:
    {
      /* Set PC to break 'end' */
      continueCtx.stmt = *ed;
      continueCtx.args = args;
      continueCtx.argVals = argVals;
      longjmp(continueJmpBuf, JMP_CODE_CONTINUE);
    }
    break;
    case LANG_ENTITY_FUNCCALL:
    {
      functionCall_t *funcCall = ((statement_t*)stmt)->content;
      call_func(
        funcCall,
        EXPRESSION_ARGS()
      );
    }
    break;
    case LANG_ENTITY_CONDITIONAL:
    {
      ifStmt_t *ifstmt = ((statement_t*)stmt)->content;
      ifStmt_t *ifstmtWalk;
      stackval_t sv;

      if ( ifstmt->ifType & LANG_CONDITIONAL_CTX ) {
        /* Handle the continue '@' and break '!@' points' */
        if ( (*(uintptr_t*)st) != (uintptr_t) stmt ) {
          PUSH_POINTER((*(uintptr_t*)st), sp, sc);
          (*(uintptr_t*)st) = (uintptr_t) stmt;
        }
        if ( (*(uintptr_t*)ed) != (uintptr_t) next ) {
          PUSH_POINTER((*(uintptr_t*)ed), sp, sc);
          (*(uintptr_t*)ed) = (uintptr_t) next;
        }
      }

      /* Read ax for conditional */
      evaluate_condition(ifstmt->cond, stmt, next, PROVIDE_CONTEXT(), args, argVals);
      if ( *ax ) {
        interpret_statements_(ifstmt->body,
          PROVIDE_CONTEXT(), args, argVals);
      } else {
        // Walk through the elifs.
        int stop = 0;
        ifstmtWalk = ifstmt->elif;

        while ( ifstmtWalk != NULL ) {
          evaluate_condition(ifstmtWalk->cond, stmt, next, PROVIDE_CONTEXT(), args, argVals);
          if ( *ax ) {
            interpret_statements_(ifstmtWalk->body,
              PROVIDE_CONTEXT(), args, argVals);
            stop = 1;
            break;
          }
          ifstmtWalk = ifstmtWalk->elif;
        }

        if ( ! stop ) {
          // Print the else if it is not NULL
          if ( ifstmt->endif != NULL ) {
            ifstmtWalk = ifstmt->endif;
            interpret_statements_(ifstmtWalk->body, PROVIDE_CONTEXT(), args, argVals);
          }
        }

      }

      if ( ifstmt->ifType & LANG_CONDITIONAL_CTX ) {
        /* Handle the continue '@' and break '!@' points' */
        POP_VAL(&sv, sp, sc);
        (*(uintptr_t*)ed) = sv.p;
        POP_VAL(&sv, sp, sc);
        (*(uintptr_t*)st) = sv.p;
      }

    }
    break;
    default:
    break;
  }

  interpret_statements_(next, PROVIDE_CONTEXT(), args, argVals);
}

void setup_namespaces() {
  funcDecs = hashtable_new(100, 0.8);
  assert(funcDecs != NULL);
  varDecs = hashtable_new(200, 0.8);
  assert(varDecs != NULL);
}

void close_namespaces() {
  hashtable_free(funcDecs);
  hashtable_free(varDecs);
}

hashtable_t *new_argstable()
{
  hashtable_t *argTable = hashtable_new(20, 0.8);
  assert(argTable != NULL);
  argTable->data_also = 1;
  return argTable;
}

void print_expr(expr_t *expr)
{
  if ( expr == NULL )
    return;

  switch (expr->type) 
  {
    case EXPR_TYPE_ID:
    printf("ID('%s')", expr->id.id);
    break;
    case EXPR_TYPE_FVAL:
    printf("%lf", expr->fval);
    break;
    case EXPR_TYPE_IVAL:
    printf("%d", expr->ival);
    break;
    case EXPR_TYPE_UVAL:
    printf("%u", expr->uval);
    break;
    case EXPR_TYPE_TEXT:
    printf("'%s'", expr->text);
    break;
    case EXPR_TYPE_VECTOR_IDX: {
      vectorIndex_t *vecIdx = expr->vecIdx;
      printf("ListIdx, ID: ");
      print_expr(vecIdx->id);
      printf(", index: ");
      print_expr(vecIdx->index);
    }
    break;
    case EXPR_TYPE_OPADD:
    printf("ADD(");
    print_expr((expr_t*)expr->add.left);
    printf(",");
    print_expr((expr_t*)expr->add.right);
    printf(")");
    break;
    case EXPR_TYPE_OPSUB:
    printf("SUB(");
    print_expr((expr_t*)expr->add.left);
    printf(",");
    print_expr((expr_t*)expr->add.right);
    printf(")");
    break;
    case EXPR_TYPE_OPMUL:
    printf("MUL(");
    print_expr((expr_t*)expr->add.left);
    printf(",");
    print_expr((expr_t*)expr->add.right);
    printf(")");
    break;
    case EXPR_TYPE_OPMOD:
    printf("MOD(");
    print_expr((expr_t*)expr->add.left);
    printf(",");
    print_expr((expr_t*)expr->add.right);
    printf(")");
    break;
    case EXPR_TYPE_OPDIV:
    printf("DIV(");
    print_expr((expr_t*)expr->add.left);
    printf(",");
    print_expr((expr_t*)expr->add.right);
    printf(")");
    break;
    case EXPR_TYPE_COND:
    printf("Conditional(");
    print_condition((ifCondition_t*)expr->cond);
    printf(")");
    break;
    case EXPR_TYPE_FUNCCALL:
    {
      functionCall_t *funcCall = expr->func;
      printf("Function call: <");
      print_expr(funcCall->id);
      printf("> args(");
      argsList_t *args = funcCall->args;
      print_args(args);
      printf(")\n");
    }
    break;
    case EXPR_TYPE_VECTOR:
    {
      vector_t *vec = (vector_t *)expr->vec;
      printf("Vector, content(");
      print_args(vec->content);
      printf(")");
    }
    break;
    case EXPR_TYPE_EMPTY:
    default:
    break;
  }

}

void print_condition(ifCondition_t *cond)
{
  if ( cond == NULL )
    return;

  printf("[");
  print_expr(cond->left);
  printf("]");

  switch ( cond->type ) {
  case CONDITION_EQ:
    printf(" == ");
    break;
  case CONDITION_NEQ:
    printf(" != ");
    break;
  case CONDITION_LEQ:
    printf(" <= ");
    break;
  case CONDITION_GEQ:
    printf(" >= ");
    break;
  case CONDITION_GE:
    printf(" > ");
    break;
  case CONDITION_LE:
    printf(" < ");
    break;

  default:
    break;
  }

  printf("[");
  print_expr(cond->right);
  printf("]");
}

void print_indents(int indent) {
  int i = 0;
  while ( i < indent ) {
    printf("    ");
    ++i;
  }
}

int print_dictionary(dictionary_t *dict,
  EXPRESSION_PARAMS()) {
  hashtable_t *hash = dict->hash;
  int size = hash->size;
  int i = 0;
  int keyCount = 0;
  int keyCountTotal = 0;
  struct key_val_pair *ptr;

  if ( dict->initialized == 0 ) {
    return -1;
  }

  i = 0;
  while ( i < size ) {
    ptr = hash->table[i];
    while (ptr != NULL) {
      keyCount++;
      ptr = ptr->next;
    }
    i++;
  }

  keyCountTotal = keyCount;

  if ( keyCountTotal > 0 ) {
    printf("<Dictionary, keys: [");
    keyCount = 0;
    i = 0;
    while ( i < size ) {
      ptr = hash->table[i];
      while (ptr != NULL) {
        printf("%s%s", (keyCount > 0 ? ", ": ""), ptr->key);
        keyCount++;
        ptr = ptr->next;
      }
      i++;
    }

    printf("]>");
  } else {
    printf("<Dictionary, no keys in it>");
  }

  return 0;
}

int print_vector(
  vector_t *vec,
  EXPRESSION_PARAMS())
{
  argsList_t *walk = vec->content;
  
  printf("[");
  while ( walk != NULL ) {
    stackval_t sv;
    evaluate_expression(walk->arg, EXPRESSION_ARGS());
    POP_VAL(&sv, sp, sc);

    switch (sv.type) {
    case INT32TYPE:
      printf("%" PRIi32 "", sv.i);
      break;
    case DOUBLETYPE:
      printf("%lf", sv.d);
      break;
    case TEXT:
      printf("'%s'", sv.t);
      break;
    case POINTERTYPE:
      printf("<%" PRIuPTR ">", sv.p);
      break;
    case FUNCPTRTYPE:
      printf("<Function: '%s'>", sv.func->id.id);
      break;
    case LIBFUNCPTRTYPE:
      printf("<Function: '%s'>", sv.func->id.id);
      break;
    case VECTORTYPE:
      print_vector(sv.vec, EXPRESSION_ARGS());
      break;
    default:
      printf("%s.error: unknown type of value on the stack (%d)\n", 
        __func__, sv.type);
      break;
    }

    walk = walk->next;
    if ( walk != NULL ) {
      printf(",");
    }
  }

  printf("]");
  
  return 0;
}

static void flush_arg(void *key, void *val)
{
  expr_t *e = (expr_t*)val;
  (void)key;
  if ( e->type == EXPR_TYPE_TEXT ) {
    free(e->text);
  } else if ( e->type == EXPR_TYPE_DICT ) {
    hashtable_free(e->dict->hash);
    free(e->dict);
  }
}

void flush_arguments(hashtable_t *table)
{
  for_each_pair(table, flush_arg);
  if ( table != NULL ) {
    hashtable_free(table);
  }
}

int print_args(argsList_t *args)
{
  if ( args == NULL )
    return 0;

  if ( print_args(args->next) ) {
    printf(",");
  }

  print_expr(args->arg);
  return 1;
}

void print_statements_(void *stmt, int indent)
{
  entity_eval_t *eval = (entity_eval_t*)stmt;
  void *next = NULL;

  if ( stmt == NULL )
    return;

  switch ( eval->entity ) {
    case LANG_ENTITY_DECL:
    case LANG_ENTITY_FUNCDECL:
    case LANG_ENTITY_FUNCCALL:
    case LANG_ENTITY_CONDITIONAL:
    case LANG_ENTITY_CONTINUE:
    case LANG_ENTITY_BREAK:
    case LANG_ENTITY_EMPTY_MATH:
    case LANG_ENTITY_EMPTY_STR:
    case LANG_ENTITY_SYSTEM:
      printf("[0x%lx] ", (uintptr_t)stmt);
      print_indents(indent);
      next = ((statement_t*)stmt)->next;
    break;
    case LANG_ENTITY_BODY:
      next = ((body_t*)stmt)->content;
      indent++;
    break;
    default:
    break;
  }

  switch ( eval->entity ) {
    case LANG_ENTITY_EMPTY_MATH:
    case LANG_ENTITY_EMPTY_STR:
    {
      printf("Expr(");
      print_expr(((statement_t*)stmt)->content);
      printf(");\n");
    }
    break;
    case LANG_ENTITY_DECL:
    {
      declaration_t* decl = ((statement_t*)stmt)->content;
      expr_t *declId = decl->id;

      switch (declId->type) {
      case EXPR_TYPE_ID: {
        printf("Declaration: ID('%s'), Expr(", declId->id.id);
        break;
      }
      case EXPR_TYPE_VECTOR_IDX: {
        printf("Declaration: ");
        print_expr(declId);
        printf(", Expr(");
        break;
      }
      default:
        fprintf(stderr, "Unexpected expression in declaration statment\n");
        GENERAL_REPORT_ISSUE_MSG();
        break;
      }
      print_expr(decl->val);
      printf(")\n");
    }
    break;
    case LANG_ENTITY_SYSTEM:
    {
      expr_t *sys_txt = ((statement_t*)stmt)->content;

      printf("System(");
      print_expr(sys_txt);
      printf(");\n");
    }
    break;
    case LANG_ENTITY_CONTINUE:
    {
      printf("=== CONTINUE ===\n");
    }
    break;
    case LANG_ENTITY_BREAK:
    {
      printf("=== BREAK ===\n");
    }
    break;
    case LANG_ENTITY_FUNCDECL:
    {
      functionDef_t *funcDef = ((statement_t*)stmt)->content;
      printf("Function Declaration: ID('%s') args(", funcDef->id.id);
      argsList_t *params = funcDef->params;
      print_args(params);
      printf(")\n");
      print_statements_(funcDef->body,indent+1);
    }
    break;
    case LANG_ENTITY_FUNCCALL:
    {
      functionCall_t *funcCall = ((statement_t*)stmt)->content;
      printf("Function call: <");
      print_expr(funcCall->id);
      printf("> args(");
      argsList_t *args = funcCall->args;
      print_args(args);
      printf(")\n");
    }
    break;
    case LANG_ENTITY_CONDITIONAL:
    {
      ifStmt_t *ifstmt = ((statement_t*)stmt)->content;
      ifStmt_t *ifstmtWalk;
      ifCondition_t *cond = ifstmt->cond;

      if ( ifstmt->ifType & LANG_CONDITIONAL_CTX ) {
        printf("loop-if-statement - condition: ");
      } else {
        printf("if-statement - condition: ");
      }

      print_condition(cond);
      printf("\n");
      print_statements_(ifstmt->body, indent);

      // Walk through the elifs.
      ifstmtWalk = ifstmt->elif;

      while ( ifstmtWalk != NULL ) {
        print_indents(indent);
        printf("else-if-statement - condition: ");
        print_condition(ifstmtWalk->cond);
        printf("\n");
        print_statements_(ifstmtWalk->body, indent+1);
        ifstmtWalk = ifstmtWalk->elif;
      }

      // Print the else if it is not NULL
      if ( ifstmt->endif != NULL ) {
        ifstmtWalk = ifstmt->endif;
        print_indents(indent);
        printf("else-statment:\n");
        print_statements_(ifstmtWalk->body, indent);
      }

    }
    break;
    default:
    break;
  }

  print_statements_(next, indent);

}

void print_statements(statement_t *stmt)
{
  print_statements_(stmt, 0);
}

void interpret_statements(
  int argc,
  char *argv[],
  statement_t *stmt
)
{
  // "CPU" registers definitions
  DEF_NEW_CONTEXT();

  // Setup stack
  SETUP_STACK(&sp, &sb, RIC_STACKSIZE, &sc);

  // Setup heap
  SETUP_HEAP(&hp, &hb, RIC_HEAPSIZE);

  // Setup namespaces
  setup_namespaces();

  // arguments to environment variables
  arguments_to_variables(argc, argv, hp);

  // Set up ric library
  initialize_ric_lib();

  /* Set starting point and end point */
  st = stmt;
  ed = NULL;

  if ( setjmp(endingJmpBuf) == JMP_CODE_INITIAL ) {
    /* Start descending and evaluating the AST */
    interpret_statements_(stmt, PROVIDE_CONTEXT_INIT(), NULL, NULL);
  } else {
    // Close namespaces
    close_namespaces();

    // free heap
    free_heap(hp, hb);

    // Free stack
    FREE_STACK(sp, sb);

    // Free memory associated with the AST
    free_ast(stmt);
  }
}

void interpret_statements_interactive(
  int argc,
  char *argv[],
  statement_t *stmt,
  int teardown
)
{
  static int firstCall = 1;
  // "CPU" registers definitions
  DEF_NEW_CONTEXT_STATIC();

  if ( firstCall ) {
    // Setup stack
    SETUP_STACK(&sp, &sb, RIC_STACKSIZE, &sc);

    // Setup heap
    SETUP_HEAP(&hp, &hb, RIC_HEAPSIZE);

    // Setup namespaces
    setup_namespaces();

    // arguments to environment variables
    arguments_to_variables(argc, argv, hp);

    // Set up ric library
    initialize_ric_lib();

    /* Set starting point and end point */
    st = stmt;
    ed = NULL;

    /* Flag that setup has been done already */
    firstCall = 0;
  }

  if ( stmt != NULL ) {

    /* Check if load */
    if ( stmt->entity == LANG_ENTITY_FUNCCALL ) {
      stackval_t stv;
      libFunction_t *libFunc;

      functionCall_t *call = (functionCall_t*)stmt->content;
      evaluate_expression(call->id, NULL, NULL, PROVIDE_CONTEXT_INIT(), NULL, NULL);

      POP_VAL(&stv, &sp, &sc);
      if ( stv.type == LIBFUNCPTRTYPE ) {
        libFunc = stv.libfunc;

        /* Check if this is a load call */
        if ( strcmp(libFunc->libFuncName, "load") == 0 ) {
          printf("Sorry, load in interactive mode is not implemented yet\n");
          return;
        }
      }

    }


    switch ( setjmp(endingJmpBuf) ) {
    case JMP_CODE_INITIAL:
        /* Start descending and evaluating the AST */
      interpret_statements_(stmt, PROVIDE_CONTEXT_INIT(), NULL, NULL);
      break;
    case JMP_CODE_TEARDOWN:
      // Close namespaces
      close_namespaces();

      // free heap
      free_heap(hp, hb);

      // Free stack
      FREE_STACK(sp, sb);

      // Free memory associated with the AST
      free_ast(stmt);
      break;
    default:
      break;
    }
  }

  if ( teardown != 0 ) {
    // Close namespaces
    close_namespaces();

    // free heap
    free_heap(hp, hb);

    // Free stack
    FREE_STACK(sp, sb);

    // Free memory associated with the AST
    free_ast(stmt);

    firstCall = 1;
  }
}

static int isAnInt(char *arg) {
  char *c = arg;
  while ( *c ) {
    if ( !isdigit(*c) ) {
      return 0;
    }
    ++c;
  }
  return 1;
}

static int isAFloat(char *arg) {
  char *c = arg;
  int nbrDots = 0;
  while ( *c ) {
    if ( !isdigit(*c) ) {
      // It might be a dot
      if ( *c != '.' ) {
        return 0;
      } else {
        if ( nbrDots == 1 ) {
          return 0;
        }
        nbrDots++;
      }
    }
    ++c;
  }

  if ( nbrDots == 1 ) {
    return 1;
  }

  return 0;
}

static int typeOfArgument(char *arg)
{
  if ( isAFloat(arg) ) {
    return DOUBLETYPE;
  }

  if ( isAnInt(arg) ) {
    return INT32TYPE;
  }

  return TEXT;
}

static char *reservedArgs[] = {
  "-pi", "-p", "-i", "-h", "--help"
};

static char *argumentListName = "args";

void arguments_to_variables(int argc, char* argv[], void *hp)
{
  /*
  * Invoking the program with command arguments:
  * --args value1 value2 ... valueN
  * will populate the global variable
  * args, which is a vector with max 10 arguments.
  */

  int heapUpdated;
  stackval_t sv;
  heapval_t *hvp = NULL;
  int argWalk = 0;
  int argCount = 0;
  int nbrReserved = sizeof(reservedArgs) / sizeof(*reservedArgs);
  int maxNbrArgs = MAX_NBR_ARGUMENTS;
  expr_t *args;
  argsList_t *argContent = NULL;

  if ( argc < 2 ) {
    /* Not OK :/ */
    return;
  }

  while ( argWalk < argc && argCount < maxNbrArgs ) {
    char *arg = argv[argWalk];
    int argWalkReserved = 0;
    int ignore = 0;
    int argType;
    expr_t *newArgExp;
    argsList_t *newArgContent;

    // Some arguments are not to be passed to the environment
    while ( argWalkReserved < nbrReserved ) {
      if ( strcmp(reservedArgs[argWalkReserved], arg) == 0 ) {
        // ignore this argument
        ignore = 1;
        break;
      }
      argWalkReserved++;
    }

    if ( ignore ) {
      argWalk++;
      continue;
    }

    newArgContent = ast_emalloc(sizeof(argsList_t));

    argType = typeOfArgument(arg);
    sv.type = argType;
    switch ( argType )
    {
    case INT32TYPE:
    {
      newArgExp = newExpr_Ival(atoi(arg));
      break;
    }
    case DOUBLETYPE:
    {
      newArgExp = newExpr_Float(atof(arg));
      break;
    }
    case TEXT:
    {
      newArgExp = newExpr_Text(arg);
      break;
    }
    default:
      fprintf(stderr, "There was something strange with the provided argument..\r\n");
      GENERAL_REPORT_ISSUE_MSG();
    break;
    }

    newArgContent->arg = newArgExp;
    if ( argContent == NULL ) {
      argContent = newArgContent;
      argContent->length = 1;
      argContent->next = NULL;
    } else {
      newArgContent->next = argContent;
      newArgContent->length = argContent->length + 1;
      argContent = newArgContent;
    }

    argWalk++;
  }

  args = newExpr_Vector(argContent);

  /* Add space for argN */
  sv.type = VECTORTYPE;
  sv.vec = args->vec;
  free(args);

  ALLOC_HEAP(&sv, hp, &hvp, &heapUpdated);

  /* Placing variable declaration in global variable namespace */
  hashtable_put(varDecs, argumentListName, hvp);
}

dictionary_t* allocNewDictionary(dictionary_t *dict, EXPRESSION_PARAMS()) {
  dictionary_t *newDict = ast_emalloc(sizeof(dictionary_t));
  newDict->hash = hashtable_new(
    DICTIONARY_STANDARD_SIZE, DICTIONARY_STANDARD_LOAD);
  newDict->hash->key_also = 1;

  if ( dict->initialized == 0 ) {
    keyValList_t *walk = dict->keyVals;
    while ( walk != NULL ) {
      /* Dictionary already initialized, evaluate expressions in the key-value list */
      expr_t *expKey = walk->key;
      expr_t *expVal = walk->val;
      char *newKeyStr = NULL;  // Storing the key
      heapval_t *hvp = NULL;  // Storing the value
      stackval_t sv;
      int dummy;  // todo: remove the need for this..

      evaluate_expression(expKey, EXPRESSION_ARGS());
      POP_VAL(&sv, sp, sc);

      switch (sv.type) {
        case TEXT: {
          size_t len = strlen(sv.t);
          newKeyStr = ast_emalloc(len+2);
          snprintf(newKeyStr, len+2, "%s", sv.t);
          break;
        }
        default:
        fprintf(stderr, "Error: Invalid dictionary expression, keys must be given as strings.\r\n");
        exit(1);
        break;
      }

      evaluate_expression(expVal, EXPRESSION_ARGS());
      POP_VAL(&sv, sp, sc);
      /* Push all values to the heap */
      switch ( sv.type ) {
      case DOUBLETYPE:
      case POINTERTYPE:
      case INT32TYPE:
      case LIBFUNCPTRTYPE:
      case FUNCPTRTYPE:
        ALLOC_HEAP(&sv, hp, &hvp, &dummy);
        break;
      case VECTORTYPE: {
        expr_t *eTemp = ast_emalloc(sizeof(expr_t));
        expr_t *newVecExpr = NULL;
        stackval_t newStackVal;

        eTemp->type = EXPR_TYPE_VECTOR;
        eTemp->vec = sv.vec;

        newVecExpr = newExpr_Copy(eTemp);
        free(eTemp);

        newStackVal = sv;
        newStackVal.vec = newVecExpr->vec;

        free(newVecExpr);

        ALLOC_HEAP(&newStackVal, hp, &hvp, &dummy);
        break;
      }
      case TEXT: {
        size_t len = strlen(sv.t);
        stackval_t newStackVal;

        char *newText = ast_emalloc(len+1);
        snprintf(newText, len+1, "%s", sv.t);

        newStackVal = sv;
        newStackVal.t = newText;

        ALLOC_HEAP(&newStackVal, hp, &hvp, &dummy);
        break;
      }
      case DICTTYPE: {
        dictionary_t *newDict = allocNewDictionary(expVal->dict, EXPRESSION_ARGS());
        stackval_t newStackVal = sv;

        newStackVal.dict = newDict;

        ALLOC_HEAP(&newStackVal, hp, &hvp, &dummy);
        break;
      }
      default:
        fprintf(stderr, "Error: Unexpected dictionary expression, value provided not valid in dictionary expressions.\r\n");
        exit(1);
        break;
      }
      /* Adding heap allocated value to dictionary hash table */
      hashtable_put(newDict->hash, newKeyStr, hvp);

      walk = walk->next;
    }
  } else {
    /* Dictionary already initialized, traverse hashtable */
    hashtable_t *hash = dict->hash;
    int size = hash->size;
    int i = 0;
    struct key_val_pair *walk;
    while ( i < size ) {
      walk = hash->table[i];
      while ( walk != NULL ) {
        // Time to evaluate the keys and the values
        char *key = walk->key;
        heapval_t *hpVal = (heapval_t*)walk->data;
        char *newKeyStr = NULL;  // Storing the key
        heapval_t *hvp = NULL;  // Storing the value
        stackval_t sv;
        int dummy;  // todo: remove the need for this..
        size_t len = strlen(key);
        newKeyStr = ast_emalloc(len+2);
        snprintf(newKeyStr, len+2, "%s", key);

        sv = hpVal->sv;
        /* Push all values to the heap */
        switch ( sv.type ) {
        case DOUBLETYPE:
        case POINTERTYPE:
        case INT32TYPE:
        case LIBFUNCPTRTYPE:
        case FUNCPTRTYPE:
          ALLOC_HEAP(&sv, hp, &hvp, &dummy);
          break;
        case VECTORTYPE: {
          expr_t *eTemp = ast_emalloc(sizeof(expr_t));
          expr_t *newVecExpr = NULL;
          stackval_t newStackVal;

          eTemp->type = EXPR_TYPE_VECTOR;
          eTemp->vec = sv.vec;

          newVecExpr = newExpr_Copy(eTemp);
          free(eTemp);

          newStackVal = sv;
          newStackVal.vec = newVecExpr->vec;

          free(newVecExpr);

          ALLOC_HEAP(&newStackVal, hp, &hvp, &dummy);
          break;
        }
        case TEXT: {
          size_t len = strlen(sv.t);
          stackval_t newStackVal;

          char *newText = ast_emalloc(len+1);
          snprintf(newText, len+1, "%s", sv.t);

          newStackVal = sv;
          newStackVal.t = newText;

          ALLOC_HEAP(&newStackVal, hp, &hvp, &dummy);
          break;
        }
        case DICTTYPE: {
          dictionary_t *newDict = allocNewDictionary(sv.dict, EXPRESSION_ARGS());
          stackval_t newStackVal = sv;

          newStackVal.dict = newDict;

          ALLOC_HEAP(&newStackVal, hp, &hvp, &dummy);
          break;
        }
        default:
          fprintf(stderr, "Error: Unexpected dictionary expression, value provided not valid in dictionary expressions.\r\n");
          exit(1);
          break;
        }
        /* Adding heap allocated value to dictionary hash table */
        hashtable_put(newDict->hash, newKeyStr, hvp);

        walk = walk->next;
      }
      i++;
    }
  }

  newDict->initialized = 1;

  return newDict;
}

