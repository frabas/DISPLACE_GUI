#ifndef NODEDATA_H
#define NODEDATA_H

#include <Node.h>

#include <QList>
#include <QVector>

#include <memory>

class DisplaceModel;

/** \brief an extension class for Node
 *
 * */
class NodeData
{
public:
    typedef QVector<int> AdiacencyList;
    typedef QVector<double> AdiacencyWeightList;

    std::shared_ptr<Node> mNode;    /* This is crap. But we have no other choice */

    explicit NodeData(std::shared_ptr<Node> nd, DisplaceModel *model);
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

    /* Adiacency functions */
    void appendAdiancency(int to_id, double weight);
    void removeAdiacencyByIdx(int idx);
    void removeAdiacencyByTarget(int target);
    void removeAllAdiacencies();
    int getAdiacencyCount() const;
    int getAdiacencyByIdx(int idx) const;
    double getAdiacencyWeight(int idx) const;
    void setAdiacencyWeight (int idx, double w);

    bool isDeleted() const;
    void setDeleted(bool value = true);

private:
    DisplaceModel *mModel;
    bool mDeleted;
    int mHarbourId;
    double *mPop;
    double mPopTot;
    double *mPopW;
    double mPopWTot;

    double *mImpact;

    AdiacencyList mAdiacency;
    AdiacencyWeightList mWeights;
};

#endif // NODEDATA_H
