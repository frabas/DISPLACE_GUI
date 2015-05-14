#ifndef COMMAND_H
#define COMMAND_H

/** \brief An abstract class that implements an interface for commands that can be undone and redone.
 *
 * This class imeplements a Command design pattern.
 * Each command that can be undone and re-done must derive from this class.
 * */
class Command
{
public:
    Command();
    virtual ~Command();

    /** \brief Execute the command.
     *
     * If the command executed correctly, the client will put the command in the Undo list.
     * \return true if the command executed correctly, false if an error occurred.*/
    virtual bool execute() = 0;

    /** \brief Undo the command.
     *
     * If the command undid correctly, the client will move the command in the Redo list. Otherwise the entire
     * Undo list must be cleaned.
     * \return true if the command executed correctly, false if an error occurred. */
    virtual bool undo() = 0;

    /** \brief Redo the command.
     *
     * If the command redid correctly, the client will move it from the redo list to the undo list.
     * \return true if the command executed correctly, false if an error occurred. */
    virtual bool redo() = 0;
};

#endif // COMMAND_H
