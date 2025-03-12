#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

@interface GameViewController : GLKViewController 
@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;
- (void)RemoveDuplicates;

@end