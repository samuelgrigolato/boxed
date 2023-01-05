//
//  http_requests.m
//  Boxed
//
//  Created by Testes on 05/01/23.
//

#import <Foundation/Foundation.h>
#import "http_requests.h"

const char * callViaCep (const char * cep) {
    NSMutableString *urlStr = [NSMutableString stringWithString:@"https://viacep.com.br/ws/"];
    [urlStr appendString:[NSString stringWithUTF8String:cep]];
    [urlStr appendString:@"/json/"];
    NSURL *url = [NSURL URLWithString:urlStr];
    NSData *data = [NSData dataWithContentsOfURL:url];
    NSString *ret = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    NSLog(@"ret=%@", ret);
    return [ret UTF8String];
}
