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
    void setPop(QList<double> v, double tot);
    double getPop(int pop) const ;
    double getPopTot () const {
        return mPopTot;
    }

private:
    DisplaceModel *mModel;
    double *mPop;
    double mPopTot;

};

#endif // NODEDATA_H
