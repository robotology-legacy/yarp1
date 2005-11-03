#import "MMFormulaNode.h"

@class MMSymbol;

/*===========================================================================

	Author: Craig-Richard Taube-Schock
		Copyright (c) 1994, Trillium Sound Research Incorporated.
		All Rights Reserved.

=============================================================================
*/

#define RULEDURATION    (-2)
#define BEAT		(-3)
#define MARK1		(-4)
#define MARK2		(-5)
#define MARK3		(-6)
#define TEMPO0		(-7)
#define TEMPO1		(-8)
#define TEMPO2		(-9)
#define TEMPO3		(-10)


@interface MMFormulaTerminal : MMFormulaNode
{
    MMSymbol *symbol;
    double value;
    int whichPhone; // TODO (2004-03-10): Rename this
}

- (id)init;
- (void)dealloc;

- (MMSymbol *)symbol;
- (void)setSymbol:(MMSymbol *)newSymbol;

- (double)value;
- (void)setValue:(double)newValue;

- (int)whichPhone;
- (void)setWhichPhone:(int)newValue;

// Methods overridden from MMFormulaNode
- (int)precedence;

- (double)evaluate:(MMFRuleSymbols *)ruleSymbols postures:(NSArray *)postures tempos:(double *)tempos;

- (int)maxPhone;

- (void)expressionString:(NSMutableString *)resultString;

@end
