#pragma once

/* JSON_checker.h */

/* Edited by Andre Leiradella to remove calls to stdlib and to (somewhat) comply with coding standards. */

struct JSONChecker
{
    int state;
    int depth;
    int top;
    int stack[ 100 ];
};

void JSONCheckerInit( JSONChecker* jc );
bool JSONCheckerChar( JSONChecker* jc, int next_char );
bool JSONCheckerDone( JSONChecker* jc );
