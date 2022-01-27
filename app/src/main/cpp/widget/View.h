//
// Created by DZSB-000848 on 1/24/22.
//

#ifndef SKIAUI_VIEW_H
#define SKIAUI_VIEW_H

#include <cstdint>
#include <yoga/Yoga.h>
#include "array"
#include "memory"
#include "core/SkPaint.h"
#include "core/SkCanvas.h"

static int64_t VIEW_ID = 0;

class View {

public:

    View();

    virtual ~View();

    int64_t viewId;

#pragma mark yoga

    virtual void measure();

    virtual void layout();

    virtual void draw(SkCanvas *canvas);

    virtual void invalidate();

    virtual void requestLayout();

    virtual void setMargins(std::array<float, 4> margins);

    virtual void setMargins(float margin);

    virtual void setPaddings(std::array<float, 4> paddings);

    virtual void setPaddings(float padding);

    virtual void setSize(float _availableWidth, float _availableHeight);

    virtual void setAlignSelf(YGAlign align);

    YGNodeRef node;

    int width, height;

    float availableWidth, availableHeight;

#pragma mark skia

    virtual void setBackgroundColor(SkColor color);

    virtual void setAntiAlias(bool antiAlias);

    virtual void setStyle(SkPaint::Style style);

    SkPaint *paint;

    std::array<int, 4> cornerRadius;

protected:
    SkRect skRect;

};

#endif //SKIAUI_VIEW_H
