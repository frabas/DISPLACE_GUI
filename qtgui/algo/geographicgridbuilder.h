#ifndef GEOGRAPHICGRIDBUILDER_H
#define GEOGRAPHICGRIDBUILDER_H

#include <QPointF>

namespace displace {

namespace graphbuilders {

/** \brief An abstract class to build a grid of points on a map, based on certain parameters
 *
 * Usage
 * This class is designed to be used in a loop. This to allow the client to reject the node creation.
 * This is how it would be used:
 *
   \verbatim
   MyBuilder builder(mysettings);
   builder.beginCreateGrid();
   while (!builder.atEnd()) {
    QPointF pt = builder.getNext();
    if (isAcceptable(pt)) {
      doSomethingUsefulWith(pt);
    }
   }
   builder.endCreateGrid();
   \endverbatim
 *
 * */
class GeographicGridBuilder
{
public:
    /** \brief Standard Destructor */
    virtual ~GeographicGridBuilder();

    /** \brief Initialize the object to iterate over the loop. */
    virtual bool beginCreateGrid() = 0;

    /** \brief Calculate the next point and iterate the object state over the loop */
    virtual QPointF getNext() = 0;

    /** \brief Check if the loop is completed
     * \return true if the loop is complete and the client's loop should exit.
     */
    virtual bool atEnd() = 0;

    /** \brief Finalize the object when the loop is completed.
     *
     * after endCreateGrid() it should be possible to call beginCreateGrid() to start a new iteration loop.
     */
    virtual bool endCreateGrid() = 0;
};

}
}

#endif // GEOGRAPHICGRIDBUILDER_H
