#import "GSParser.h"

@class MMBooleanNode, MMCategory, MModel;

/*===========================================================================

	Author: Craig-Richard Taube-Schock
		Copyright (c) 1994, Trillium Sound Research Incorporated.
		All Rights Reserved.

=============================================================================

	Object: BooleanParser
	Purpose: To parse a boolean expression string and build a boolean
		expression tree.

	Instance Variables:
		consumed: (int) currently not used.  May be used for look-
			ahead parsing.
		parseString: (const char *) The string being parsed.  NOTE
			that it is const and should not be modified.
		symbolString: (char[256]) Buffer for the current symbol.

		categoryList: In MONET, terminals for the boolean expression
			system are instances of the MMCategory class.
			The majority of those instances are stored in a
			named object which is of the "CategoryList" class.
			When a category symbol is to be resolved, this list
			is consulted.
		phoneList:  Not all MMCategorys are stored in the
			mainCategoryList.  Some are categories native to a
			specific phone.  If a category cannot be found in the
			main category list, the main phone list is consulted.

	"BooleanSymbols.h" for some TOKEN defines.
*/

@interface MMBooleanParser : GSParser
{
    MModel *model;
}

- (id)initWithModel:(MModel *)aModel;
- (void)dealloc;

- (MModel *)model;
- (void)setModel:(MModel *)newModel;

/* General purpose internal methods */
- (MMCategory *)categoryWithName:(NSString *)aName;
- (int)nextToken;

/* General Parse Methods */
- (id)beginParseString;
- (MMBooleanNode *)continueParse:(MMBooleanNode *)currentExpression;

/* Internal recursive descent methods */
- (MMBooleanNode *)notOperation;
- (MMBooleanNode *)andOperation:(MMBooleanNode *)operand;
- (MMBooleanNode *)orOperation:(MMBooleanNode *)operand;
- (MMBooleanNode *)xorOperation:(MMBooleanNode *)operand;

- (MMBooleanNode *)leftParen;

@end
