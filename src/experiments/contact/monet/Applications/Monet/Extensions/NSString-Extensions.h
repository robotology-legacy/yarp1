//  This file is part of class-dump, a utility for examining the
//  Objective-C segment of Mach-O files.
//  Copyright (C) 1997-1998, 2000-2001, 2004  Steve Nygard

#import <Foundation/NSString.h>

@interface NSString (CDExtensions)

+ (NSString *)stringWithFileSystemRepresentation:(const char *)str;
+ (NSString *)spacesIndentedToLevel:(int)level;
+ (NSString *)spacesIndentedToLevel:(int)level spacesPerLevel:(int)spacesPerLevel;
+ (NSString *)spacesOfLength:(int)targetLength;
+ (NSString *)stringWithUnichar:(unichar)character;

- (BOOL)isFirstLetterUppercase;
- (BOOL)hasPrefix:(NSString *)aString ignoreCase:(BOOL)shouldIgnoreCase;

+ (NSString *)stringWithASCIICString:(const char *)bytes;

- (NSString *)leftJustifiedStringPaddedToLength:(int)paddedLength;
- (NSString *)rightJustifiedStringPaddedToLength:(int)paddedLength;

@end

@interface NSMutableString (Extensions)

- (void)indentToLevel:(int)level;

@end
