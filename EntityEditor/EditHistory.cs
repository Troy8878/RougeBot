using System.Collections.Generic;
using System.Linq;

namespace EntityEditor
{
    public class EditHistory
    {
        public delegate UndoAction RedoAction();

        public delegate RedoAction UndoAction();

        private static readonly Stack<UndoAction> UndoStack = new Stack<UndoAction>();
        private static readonly Stack<RedoAction> RedoStack = new Stack<RedoAction>();

        public static void PushUndo(UndoAction action)
        {
            UndoStack.Push(action);
        }

        public static void Undo()
        {
            if (!UndoStack.Any())
                return;

            RedoStack.Push(UndoStack.Pop()());
        }

        public static void Redo()
        {
            if (!RedoStack.Any())
                return;

            UndoStack.Push(RedoStack.Pop()());
        }
    }
}