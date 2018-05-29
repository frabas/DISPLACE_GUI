//
// Created by Federico Fuga on 29/05/18.
//

#ifndef DISPLACE_STATSPLOT_H
#define DISPLACE_STATSPLOT_H

class DisplaceModel;
class QCustomPlot;

class StatsPlot {
    bool needsUpdate = true;
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
protected:
    virtual void doUpdate () = 0;
};


#endif //DISPLACE_STATSPLOT_H
