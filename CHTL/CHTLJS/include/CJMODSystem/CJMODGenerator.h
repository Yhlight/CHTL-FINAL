#ifndef CJMODGENERATOR_H
#define CJMODGENERATOR_H

#include "Arg.h"
#include <string>

/**
 * @class CJMODGenerator
 * @brief Responsible for generating the final output from a transformed Arg object.
 */
class CJMODGenerator {
public:
    /**
     * @brief Exports the final transformed result.
     * @param args The Arg object containing the transformed output.
     */
    static void exportResult(const Arg& args);
};

#endif // CJMODGENERATOR_H
