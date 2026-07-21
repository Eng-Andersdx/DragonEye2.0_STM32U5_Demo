#include <gui/intro_screen/IntroView.hpp>

#include <BitmapDatabase.hpp>

#include <initializer_list>


IntroView::IntroView() :
    introAnimationSequence(ANIMATE_BACKGROUND),
    animate(false),
    stepCounter(0),
    steps(0),
    stepCounter2(0),
    steps2(0),
    startValue(0.f),
    startValue2(0.f),
    stopValue(0.f),
    stopValue2(0.f)
{

}

void IntroView::setupScreen()
{
    IntroViewBase::setupScreen();

    HAL::getInstance()->setFrameRateCompensation(true);
}

void IntroView::tearDownScreen()
{
    IntroViewBase::tearDownScreen();
}

void IntroView::handleTickEvent()
{
    if (animate)
    {
        // Declaration of all the variables necessary for the animations' calculations
        float scale;
        float scale2;
        float zAngle;
        float zAngle2;

        switch (introAnimationSequence)
        {
        // The ANIMATE_BACKGROUND is the first animation step when entering the "Intro" screen
        case ANIMATE_BACKGROUND:

            // The background pattern image, used in a texture mapper widget (see the TextureMapper widget article in the online TouchGFX documentation), is scaled in from 0 to 100% of its size in a set number of steps
            scale = floatEaseOut((float)stepCounter, 0.f, 1.f, (float)steps);
            backgroundPattern.setVisible(true);
            backgroundPattern.setScale(scale);
            backgroundPattern.invalidateContent();

            stepCounter++;

            // Once the background pattern image scalling is finished the ring image is faded in to an alpha value of 255 (100%). An Image widget using the background image is set to visible instead of the Texture Mapper widget in order to increase the performances by reducing the amount of UI elements requiring computation (see the Image widget article in the online TouchGFX documentation ). The animation sequence moves to the next step called ANIMATE_RING
            if (stepCounter > steps)
            {
                //                backgroundRingTextureMapper.startFadeAnimation(255, 50, EasingEquations::cubicEaseOut);

                backgroundPatternImage.setVisible(true);
                backgroundPatternImage.invalidate();
                backgroundPattern.setVisible(false);
                backgroundPattern.invalidate();

                stepCounter = 0;
                steps = ANIMATE_RING_DURATION;
                introAnimationSequence = ANIMATE_RING;
            }

            break;

        case ANIMATE_RING:

            // Workaround: The backgroundRingTextureMapper is taken out for now due to flikering on the board. Comment in this when the reason is found.
            //            // The ring image, used in a texture mapper widget, is scalled out from 85 to 15% of its size in a set number of steps
            //            scale = floatEaseOut((float)stepCounter, 0.85f, 0.15f, (float)steps);
            //            backgroundRingTextureMapper.setScale(scale);
            //            backgroundRingTextureMapper.invalidateContent();

            stepCounter++;

            // Once the background ring animation is finished. The texture mapper widget using the background ring image is replaced by an image widget to increase the performances. The animation sequence moves to the next step called ANIMATE_CENTER_CIRCLE
            if (stepCounter > steps)
            {

                backgroundRingImage.setVisible(true);
                backgroundRingImage.invalidate();
                //                backgroundRingTextureMapper.setVisible(false);
                //                backgroundRingTextureMapper.invalidate();

                dot.setVisible(true);

                stepCounter = 0;
                steps = ANIMATE_CENTER_CIRCLE_DURATION;
                introAnimationSequence = ANIMATE_CENTER_CIRCLE;
            }
            break;

        case ANIMATE_CENTER_CIRCLE:

            // The dot image, used in a texture mapper widget, is scalled in from 0 to 100% of its size in a set number of steps
            scale = floatBackEaseOut((float)stepCounter, 0.f, 1.f, (float)steps);
            dot.setScale(scale);
            dot.invalidateContent();

            stepCounter++;

            // Once the dot image animation is finished the first flower image, used in a texture mapper widget called topFlower, is set to visible while its initial scale is of 0% of its size. The animation sequence moves to the next step called ANIMATE_FLOWER_SCALE_UP
            if (stepCounter > steps)
            {
                topFlower.setVisible(true);

                stepCounter = 0;
                steps = ANIMATE_FLOWER_SCALE_UP_DURATION;
                introAnimationSequence = ANIMATE_FLOWER_SCALE_UP;
            }
            break;

        case ANIMATE_FLOWER_SCALE_UP:

            // The flower image called bottom flower is scalled in from 0 to 100% of its size and rotated in a set number of steps
            if (stepCounter > steps / 2)
            {
                scale2 = floatEaseOut((float)stepCounter2, 0.f, 1.f, (float)steps2);
                zAngle2 = floatEaseOut((float)stepCounter2, 0.f, 1.571f, (float)steps2); // The end value 1.571f will have the flower do a clockwise rotation of pi/2 (90 degree)

                bottomFlower.updateZAngle(zAngle2);
                bottomFlower.setScale(scale2);
                bottomFlower.invalidateContent();

                stepCounter2++;
            }

            scale = floatEaseOut((float)stepCounter, 0.f, 1.f, (float)steps);
            zAngle = floatEaseOut((float)stepCounter, 0.f, -3.1416f, (float)steps); // The end value -3.141f will have the top flower do a counterclockwise rotation of pi (180 degree)

            topFlower.updateZAngle(zAngle);
            topFlower.setScale(scale);
            topFlower.invalidateContent();

            // The texture mapper widget using the shadow image follows the same scale as the top flower
            shadow.setScale(scale);
            shadow.invalidateContent();

            stepCounter++;

            if (stepCounter == steps / 2)
            {
                bottomFlower.setVisible(true);
                stepCounter2 = 0;
                steps2 = steps / 2;
            }
            else if (stepCounter > steps)
            {
                startValue = topFlower.getZAngle();
                stopValue = 3.1416f;

                // Replacing the texture mapper widgets using the background image and the shadow image by a single image widget to improve performances
                backgroundImage.setVisible(true);
                backgroundImage.invalidate();
                shadow.setVisible(false);
                shadow.invalidate();
                backgroundRingImage.setVisible(false);
                backgroundRingImage.invalidate();
                backgroundPatternImage.setVisible(false);
                backgroundPatternImage.invalidate();

                stepCounter = 0;
                steps = ANIMATE_FLOWER_ROTATE_DURATION;
                introAnimationSequence = ANIMATE_FLOWER_ROTATE;
            }
            break;

        case ANIMATE_FLOWER_ROTATE:

            if (stepCounter > steps / 8)        // Rotate the flowers the other way. The bottom flower will initialy do not rotate until the step counter reaches 1/8th of the total number of steps
            {
                zAngle2 = floatEaseOut((float)stepCounter2, startValue2, stopValue2, (float)steps2); // The initial startValue2 is 0f and the stopValue2 is 0f. The bottom flower will not rotate until the values of startValue2 and stopValue2 are modified

                bottomFlower.updateZAngle(zAngle2);
                bottomFlower.invalidateContent();

                stepCounter2++;
            }

            zAngle = floatEaseOut((float)stepCounter, startValue, stopValue, (float)steps); // The initial startValue is -3.1416f and the stopValue is 3.1416f. The top flower will do an initial rotation clockwise of 2 pi (360 degree)

            topFlower.updateZAngle(zAngle);
            topFlower.invalidateContent();

            stepCounter++;

            if (stepCounter == steps / 8)       // Modifies the values of startValue2 and stopValue2 to start the rotation of the bottom flower. The value of startValue2 is 1.571f and stopValue2 is -2.3562f. The bottom flower will then do a rotation counterclockwise of pi (180 degree)
            {
                startValue2 = bottomFlower.getZAngle();
                // stopValue2 = -2.3562f;
                stopValue2 = -1.571f;
                stepCounter2 = 0;
                steps2 = steps / 4 * 3;
            }

            if (stepCounter > steps)
            {
                startValue = topFlower.getZAngle();     // The value of startValue is the current value of the angle instead of a set angle to prevent a glitch behavior if the values were different
                stopValue = 0.7853f * 2;

                // The shadow will scale follow the same animation as the top flower
                shadow.setVisible(true);
                shadow.invalidate();
                backgroundRingImage.setVisible(true);
                backgroundRingImage.invalidate();
                backgroundPatternImage.setVisible(true);
                backgroundPatternImage.invalidate();
                backgroundImage.setVisible(false);
                backgroundImage.invalidate();

                stepCounter = 0;
                steps = ANIMATE_FLOWER_SCALE_DOWN_DURATION;
                introAnimationSequence = ANIMATE_FLOWER_SCALE_DOWN;

            }
            break;

        case ANIMATE_FLOWER_SCALE_DOWN:

            // The flowers are both scaled down until they are invisible to prepare for the screen transition
            if (stepCounter > steps / 4)
            {
                scale2 = floatEaseOut((float)stepCounter2, 1.f, -1.f, (float)steps2);
                zAngle2 = floatEaseOut((float)stepCounter2, startValue2, stopValue2, (float)steps2);

                bottomFlower.updateZAngle(zAngle2);
                bottomFlower.setScale(scale2);
                bottomFlower.invalidateContent();

                stepCounter2++;
            }

            scale = floatEaseOut((float)stepCounter, 1.f, -1.f, (float)steps);
            zAngle = floatEaseOut((float)stepCounter, startValue, stopValue, (float)steps);

            topFlower.updateZAngle(zAngle);
            topFlower.setScale(scale);
            topFlower.invalidateContent();

            shadow.setScale(scale);
            shadow.invalidateContent();

            stepCounter++;

            if (stepCounter == steps / 4)
            {
                startValue2 = bottomFlower.getZAngle();
                stopValue2 = 0.3927f * 2;
                stepCounter2 = 0;
                steps2 = steps / 4 * 3;
            }

            if (stepCounter > steps)
            {
                animate = false;

                shadow.setVisible(false);
                shadow.invalidate();

                // The final animation of the dot is called. Once finished it will trigger the interaction ChangeScreenToMain (see Interactions tab in TouchGFX Designer). This interaction will trigger a screen change animation to the screen called "Main"
                IntroViewBase::moveDot();
            }
            break;
        }
    }
}


/**
 * Initialize the graphics and the settings when the screen is entered.
 * The images and texture mappers are initally invisible to the user by either setting the visibility of the widgets to 0 or by setting the scale to 0.
 */
void IntroView::screenIsEntered()
{
    // Make the images invisible at start up
    backgroundRingImage.setVisible(false);
    backgroundImage.setVisible(false);
    backgroundPattern.setVisible(false);
    backgroundPatternImage.setVisible(false);
    backgroundRingTextureMapper.setVisible(true);
    backgroundRingTextureMapper.setAlpha(0);
    shadow.setVisible(true);
    shadow.setScale(0);
    dot.setScale(0);
    topFlower.setScale(0);
    bottomFlower.setScale(0);

    // Invalidate the changes made to the texture mapper widgets
    backgroundImage.invalidate();
    backgroundPattern.invalidate();
    backgroundPatternImage.invalidate();
    backgroundRingImage.invalidate();
    backgroundRingTextureMapper.invalidate();
    shadow.invalidate();
    dot.invalidate();
    topFlower.invalidate();
    bottomFlower.invalidate();

    // Launch the animation
    animate = true;
    stepCounter = 0;
    steps = ANIMATE_BACKGROUND_DURATION;
}


/**
 * The floatEaseOut function is used to smoothen the setScale animation by giving a set value of scales depending on the duration (amount of steps) and the begining and ending scale values required. It will have a slow start and then accelerate towards the end
 */
float IntroView::floatEaseOut(float tempoStepCounter, float tempoStartValue, float tempoEndValue, float tempoNumberOfSteps)
{
    tempoStepCounter = tempoStepCounter / tempoNumberOfSteps - 1;
    return tempoEndValue * (tempoStepCounter * tempoStepCounter * tempoStepCounter + 1) + tempoStartValue;
}


/**
 * The floatBackEaseOut function is used to smoothen the setScale animation by giving a set value of scales depending on the duration (amount of steps) and the begining and ending scale values required. It will have a fast start and will slow down towards the end
 */
float IntroView::floatBackEaseOut(float tempoStepCounter, float tempoStartValue, float tempoEndValue, float tempoNumberOfSteps)
{
    float s = 1.70158f;
    tempoStepCounter = tempoStepCounter / tempoNumberOfSteps - 1;
    return tempoEndValue * (tempoStepCounter * tempoStepCounter * ((s + 1) * tempoStepCounter + s) + 1) + tempoStartValue;
}
