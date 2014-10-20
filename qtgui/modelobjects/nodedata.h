#ifndef NODEDATA_H
#define NODEDATA_H

#include <Node.h>

#include <QList>

class DisplaceModel;

/** \brief an extension class for Node
 *
 * */
class NodeData
{
public:
    Node *mNode;    /* This is crap. But we have no other choice */

    explicit NodeData(Node *nd, DisplaceModel *model);
    virtual ~NodeData();

    /* Proxy functions to avoid change too much code */
    int get_idx_node() const { return mNode->get_idx_node(); }
    int get_marine_landscape() const { return mNode->get_marine_landscape(); }
    int get_code_area() const { return mNode->get_code_area(); }
    virtual string get_name() const { return mNode->get_name(); }
    double get_x() const { return mNode->get_x(); }
    double get_y() const { return mNode->get_y(); }
    int get_harbour() const { return mNode->get_harbour(); }
    bool get_is_harbour() const { return mNode->get_is_harbour(); }
    int get_cumftime() const { return mNode->get_cumftime(); }
    void set_cumftime(int i) { mNode->set_cumftime(i); }

    /* Extending functions */

    DisplaceModel *getModel() const { return mModel; }

    int getPopCount() const;
    void setPop(int pop, double v);
    void setPopTot(double tot);
    void setPop(QList<double> v, double tot);
    double getPop(int pop) const ;
    double getPopTot () const {
        return mPopTot;
    }

    void setPopW(int pop, double val);
    void setPopWTot(double tot);
    void setPopW(QList<double> v, double tot);
    double getPopW(int pop) const ;
    double getPopWTot () const {
        return mPopWTot;
    }

    void setImpact(int pop, double impact);
    double getImpact(int pop) const {
        return mImpact[pop];
    }

    int getHarbourId() const;
    void setHarbourId(int value);

private:
    DisplaceModel *mModel;
    int mHarbourId;
    double *mPop;
    double mPopTot;
    double *mPopW;
    double mPopWTot;

    double *mImpact;
};

#endif // NODEDATA_H
