//
// Created by Federico Fuga on 29/05/18.
//

#ifndef DISPLACE_STATSPLOT_H
#define DISPLACE_STATSPLOT_H

#include <qtcommons_global.h>

class DisplaceModel;
class QCustomPlot;

class QTCOMMONSSHARED_EXPORT StatsPlot {
    bool needsUpdate = true;
    bool mVisible = false;
public:
    virtual ~StatsPlot() noexcept = default;

    virtual void update() {
        if (needsUpdate) {
            doUpdate();
            needsUpdate = false;
        }
    }

    void invalidate() {
        needsUpdate = true;
    }
    void setVisible (bool visible) {
        mVisible = visible;
    }
    bool isVisible() const { return mVisible; }
protected:
    virtual void doUpdate () = 0;
};


#endif //DISPLACE_STATSPLOT_H
