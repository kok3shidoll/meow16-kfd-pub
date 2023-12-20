//
//  ViewController.m
//  kfd-meow
//
//  Created by doraaa on 2023/12/14.
//

#import "ViewController.h"
#import "libkfd.h"
#import "libmeow.h"

uint64_t _kfd = 0;

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (IBAction)meow:(id)sender {
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        _kfd = kopen(MEOW_EXPLOIT_SMITH);
        
        if(_kfd) {
            
            NSLog(@"Success");
            
            meow();
            
            if(_kfd) {
                kclose(_kfd);
            }
        }
        
    });
}

@end
