#import <Foundation/NSObject.h>

#import "MMFRuleSymbols.h"
#import "EventList.h"

@class MonetList;
@class MMPoint, MMSlope, NSMutableArray;
@class NSXMLParser;

/*===========================================================================

	Author: Craig-Richard Taube-Schock
		Copyright (c) 1994, Trillium Sound Research Incorporated.
		All Rights Reserved.

=============================================================================
*/

@interface MMSlopeRatio : NSObject
{
    NSMutableArray *points; // Of MMPoints
    NSMutableArray *slopes; // Of MMSlopes
}

- (id)init;
- (void)dealloc;

- (NSMutableArray *)points;
- (void)setPoints:(NSMutableArray *)newList;
- (void)addPoint:(MMPoint *)newPoint;

- (NSMutableArray *)slopes;
- (void)setSlopes:(NSMutableArray *)newList;
- (void)addSlope:(MMSlope *)newSlope;
- (void)updateSlopes;

- (double)startTime;
- (double)endTime;

- (void)calculatePoints:(MMFRuleSymbols *)ruleSymbols tempos:(double *)tempos postures:(NSArray *)postures andCacheWith:(int)newCacheTag
              toDisplay:(MonetList *)displayList;

- (double)calculatePoints:(MMFRuleSymbols *)ruleSymbols tempos:(double *)tempos postures:(NSArray *)postures andCacheWith:(int)newCacheTag
                 baseline:(double)baseline delta:(double)parameterDelta min:(double)min max:(double)max
              toEventList:(EventList *)eventList atIndex:(int)index;

- (double)totalSlopeUnits;
- (void)displaySlopesInList:(NSMutableArray *)displaySlopes;

// Archiving
- (id)initWithCoder:(NSCoder *)aDecoder;

- (NSString *)description;

- (void)appendXMLToString:(NSMutableString *)resultString level:(int)level;

- (id)initWithXMLAttributes:(NSDictionary *)attributes context:(id)context;
- (void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName attributes:(NSDictionary *)attributeDict;
- (void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName;

@end
