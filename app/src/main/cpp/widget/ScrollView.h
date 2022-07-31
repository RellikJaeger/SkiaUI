//
// Created by ByteDance on 2022/7/15.
//

#ifndef SKIAUI_SCROLLVIEW_H
#define SKIAUI_SCROLLVIEW_H


#include "FlexboxLayout.h"

/**
 * 如果是wrap 高度=子View高度之和，不支持嵌套ScrollView
 */
class ScrollView : public FlexboxLayout {

public:
    constexpr static int MIN_VELOCITY = 50;
    constexpr static int MAX_VELOCITY = 8000;
    constexpr static float FLING_FRICTION = 0.015f;
    static float DECELERATION_RATE;
    constexpr static float INFLEXION = 0.35f;
    constexpr static float GRAVITY = 9.8f;

public:

    ScrollView();

    virtual ~ScrollView();

    virtual void measure(int widthMeasureSpec, int heightMeasureSpec) override;

    virtual void layout(int l, int t, int r, int b) override;

    virtual void updateTranslateY(float diffY);

    virtual void updateTranslateX(float diffX);

    virtual void setFlexWrap(YGWrap wrap) override;

    virtual void setFlexDirection(YGFlexDirection direction) override;

    virtual bool addView(View *view, LayoutParams *layoutParams) override;

    virtual bool removeView(View *view) override;

    virtual bool canScroll();

    void setVelocity(float x, float y);

    void startFling();

    void stopFling();

    void addScrollCallback(std::function<void(float dx, float dy)> callback);

protected:

    float xVelocity, yVelocity;

    float calculateFlingTranslate();

    bool isFling;

    long startTime;

    std::vector<std::function<void(float dx, float dy)>> scrollCallbacks;

};


#endif //SKIAUI_SCROLLVIEW_H
