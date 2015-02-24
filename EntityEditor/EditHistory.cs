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
            RedoStack.Clear();
        }

        public static void ClearHistory()
        {
            UndoStack.Clear();
            RedoStack.Clear();
        }

        public static bool CanUndo()
        {
            return UndoStack.Any();
        }

        public static bool CanRedo()
        {
            return RedoStack.Any();
        }

        public static void Undo()
        {
            if (!UndoStack.Any())
                return;

            var next = UndoStack.Pop()();
            if (next != null)
                RedoStack.Push(next);
        }

        public static void Redo()
        {
            if (!RedoStack.Any())
                return;

            var next = RedoStack.Pop()();
            if (next != null)
                UndoStack.Push(next);
        }
    }
}