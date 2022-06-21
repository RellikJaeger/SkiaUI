//
// Created by DZSB-000848 on 1/24/22.
//

#include <yoga/Utils.h>
#include <base/native_log.h>
#include "ViewGroup.h"

ViewGroup::ViewGroup() : View(), maxChildWidth(0.0f), maxChildHeight(0.0f) {

}

ViewGroup::~ViewGroup() {
    if (node == nullptr) {
        return;
    }
    YGNodeRemoveAllChildren(node);
    for (auto view: children) {
        delete view;
    }
}

bool ViewGroup::addView(View *view, LayoutParams *layoutParams) {
    if (view == nullptr || view->node == nullptr) {
        ALOGE("add null view, pls check view!")
        return false;
    }
    auto childCount = YGNodeGetChildCount(node);
    ALOGD("addChildView %s at index %d", view->name(), childCount);
//    if (view->node->getOwner() != nullptr) {
//        ALOGE("addChildView error node->getOwner() != null, pls check")
//        view->node->setOwner(nullptr);
//    }
    YGNodeInsertChild(node, view->node, childCount);
    children.emplace_back(view);
    view->setLayoutParams(layoutParams);
    return true;
}

bool ViewGroup::removeView(View *view) {
    if (view == nullptr || view->node == nullptr) {
        ALOGE("remove null view, pls check view!")
        return false;
    }
    YGNodeRemoveChild(node, view->node);
    for (auto ite = children.begin(); ite < children.end(); ++ite) {
        if ((*ite)->viewId == view->viewId) {
            children.erase(ite);
            delete *ite;
            return true;
        }
    }
    return false;
}

void ViewGroup::removeAllViews() {
    if (node == nullptr) {
        ALOGE("remove null view, pls check view!")
        return;
    }
    for (auto view: children) {
        delete view;
    }
}

void ViewGroup::measure(int widthMeasureSpec, int heightMeasureSpec) {
    //todo ViewGroup的measure方法不对
    for (auto &child: children) {
        measureChild(child, widthMeasureSpec, heightMeasureSpec);
    }
    YGNodeCalculateLayout(node, MeasureSpec::getSize(widthMeasureSpec),
                          MeasureSpec::getSize(heightMeasureSpec),
                          YGDirectionLTR);
    if (isViewGroup()) {
        if (strcmp(this->name(), "FlexboxLayout") == 0) {
            ALOGD("777777 %d %d", layoutParams->_widthMode == EXACTLY,
                  layoutParams->_heightMode == EXACTLY)
        }
        if (layoutParams->_widthMode == EXACTLY) {
            YGNodeStyleSetWidth(node, static_cast<float>(layoutParams->_width));
        }
        if (layoutParams->_heightMode == EXACTLY) {
            YGNodeStyleSetHeight(node, static_cast<float>(layoutParams->_height));
        }
    }
}

void ViewGroup::setMeasuredDimension(int _measuredWidth, int _measuredHeight) {
    width = _measuredWidth;
    height = _measuredHeight;
    ALOGD("ViewGroup setMeasuredDimension %s %d %d", name(), _measuredWidth, _measuredHeight)
    YGNodeStyleSetWidth(node, static_cast<float>(_measuredWidth));
    YGNodeStyleSetHeight(node, static_cast<float>(_measuredHeight));
}

void ViewGroup::measureChild(View *child, int parentWidthMeasureSpec,
                             int parentHeightMeasureSpec) {
    auto lp = layoutParams.get();
    auto childWidthMeasureSpec = getChildMeasureSpec(child, parentWidthMeasureSpec,
                                                     lp->_paddingStart + lp->_paddingEnd,
                                                     child->layoutParams->_width);
    auto childHeightMeasureSpec = getChildMeasureSpec(child, parentHeightMeasureSpec,
                                                      lp->_paddingTop + lp->_paddingBottom,
                                                      child->layoutParams->_height);
    ALOGD("measureChild %s %d %d", child->name(), MeasureSpec::getSize(childWidthMeasureSpec),
          MeasureSpec::getSize(childHeightMeasureSpec));
    child->measure(childWidthMeasureSpec, childHeightMeasureSpec);
}

int ViewGroup::getChildMeasureSpec(View *child, int parentMeasureSpec, int padding,
                                   int childDimension) {
    MeasureSpec::toString(parentMeasureSpec);
    auto specMode = MeasureSpec::getMode(parentMeasureSpec);
    auto specSize = MeasureSpec::getSize(parentMeasureSpec);
    auto size = std::max(0, specSize - padding);
    auto resultSize = 0;
    auto resultMode = UNSPECIFIED;
    switch (specMode) {
        // Parent has imposed an exact size on us
        case EXACTLY: {
            ALOGD("getChildMeasureSpec exactly %s %d", child->name(), childDimension)
            if (childDimension >= 0) {
                resultSize = childDimension;
                resultMode = EXACTLY;
            } else if (MeasureSpec::isMatchParent(childDimension)) {
                // Child wants to be our size. So be it.
                resultSize = size;
                resultMode = EXACTLY;
            } else {
                // Child wants to determine its own size. It can't be
                // bigger than us.
                resultSize = size;
                resultMode = AT_MOST;
            }
            break;
        }
            // Parent has imposed a maximum size on us
        case AT_MOST: {
            if (childDimension >= 0) {
                // Child wants a specific size... so be it
                resultSize = childDimension;
                resultMode = EXACTLY;
            } else if (MeasureSpec::isMatchParent(childDimension)) {
                // Child wants to be our size, but our size is not fixed.
                // Constrain child to not be bigger than us.
                resultSize = size;
                resultMode = AT_MOST;
            } else {
                // Child wants to determine its own size. It can't be
                // bigger than us.
                resultSize = size;
                resultMode = AT_MOST;
            }
            break;
        }

            // Parent asked to see how big we want to be
        case UNSPECIFIED: {
            if (childDimension >= 0) {
                // Child wants a specific size... let him have it
                resultSize = childDimension;
                resultMode = EXACTLY;
            } else if (MeasureSpec::isMatchParent(childDimension)) {
                // Child wants to be our size... find out how big it should be
                resultSize = size;
                resultMode = UNSPECIFIED;
            } else {
                // Child wants to determine its own size.... find out how big it should be
                resultSize = size;
                resultMode = UNSPECIFIED;
            }
            break;
        }
        default: {
            ALOGD("illegal specMode")
            assert(nullptr);
        }
    }
    return MeasureSpec::makeMeasureSpec(resultSize, resultMode);
}

void ViewGroup::draw(SkCanvas *canvas) {
    View::draw(canvas);
    for (auto child: children) {
        child->draw(canvas);
    }
}

void ViewGroup::setAlignItems(YGAlign align) {
    SkASSERT(node);
    if (node == nullptr) {
        return;
    }
    YGNodeStyleSetAlignItems(node, align);
}

void ViewGroup::setJustifyContent(YGJustify justify) {
    SkASSERT(node);
    if (node == nullptr) {
        return;
    }
    YGNodeStyleSetJustifyContent(node, justify);
}

void ViewGroup::setAlignContent(YGAlign align) {
    SkASSERT(node);
    if (node == nullptr) {
        return;
    }
    YGNodeStyleSetAlignContent(node, align);
}

void ViewGroup::setFlexWrap(YGWrap wrap) {
    SkASSERT(node);
    if (node == nullptr) {
        return;
    }
    YGNodeStyleSetFlexWrap(node, wrap);
}

void ViewGroup::setFlexDirection(YGFlexDirection direction) {
    SkASSERT(node);
    if (node == nullptr) {
        return;
    }
    YGNodeStyleSetFlexDirection(node, direction);
}

int ViewGroup::getWidth() {
    if (YGNodeStyleGetWidth(node).unit == YGUnitAuto) {
        return getMaxWidthInChildren();
    } else {
        //exactly todo percent
        return View::getWidth();
    }
}

int ViewGroup::getHeight() {
    if (YGNodeStyleGetHeight(node).unit == YGUnitAuto) {
        return getMaxHeightInChildren();
    } else {
        //exactly todo percent
        return View::getHeight();
    }
}

int ViewGroup::getMaxHeightInChildren() {
    int maxHeight = 0;
    for (auto &child: children) {
        maxHeight = std::max(maxHeight, child->getHeight());
    }
    return maxHeight;
}

int ViewGroup::getMaxWidthInChildren() {
    int maxWidth = 0;
    for (auto &child: children) {
        maxWidth = std::max(maxWidth, child->getWidth());
    }
    return maxWidth;
}

bool ViewGroup::isViewGroup() {
    return true;
}

const char *ViewGroup::name() {
    return "ViewGroup";
}

YGConfigRef ViewGroup::getConfig() {
    return config;
}

bool ViewGroup::dispatchTouchEvent(TouchEvent *touchEvent) {
    return touchEventDispatcher->dispatchTouchEvent(touchEvent);
}

bool ViewGroup::onInterceptTouchEvent(TouchEvent *touchEvent) {
    return touchEventDispatcher->onInterceptTouchEvent(touchEvent);
}

bool ViewGroup::onTouchEvent(TouchEvent *touchEvent) {
    return touchEventDispatcher->onTouchEvent(touchEvent);
}

void ViewGroup::requestDisallowInterceptTouchEvent(bool disallowIntercept) {
    touchEventDispatcher->requestDisallowInterceptTouchEvent(disallowIntercept);
}

