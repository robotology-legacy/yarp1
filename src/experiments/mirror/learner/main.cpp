#include <iostream>

#include "learningMachine.h"

int main ( int argc, char** argv )
{

//    LearningMachine<msNormaliser>::paramsType p(100,3,"prova");
//    LearningMachine<msNormaliser> m(p);

    LearningMachine<mmNormaliser>::paramsType p(100,3,"prova");
    LearningMachine<mmNormaliser> m(p);

//    LearningMachine<nullNormaliser>::paramsType p(100,3,"prova");
//    LearningMachine<nullNormaliser> m(p);

    m.load();
    m.save();

    return 0;

}
