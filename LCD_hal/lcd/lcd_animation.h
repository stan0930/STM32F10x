#ifndef LCD_ANIMATION_H
#define LCD_ANIMATION_H

/* Frame period in milliseconds: 80 ms = 12.5 frames/second. */
#define LCD_ANIMATION_FRAME_MS 80U

/* Call once after LCD_Init(), then call Task() from the main loop. */
void LCD_AnimationInit(void);
void LCD_AnimationTask(void);

#endif
