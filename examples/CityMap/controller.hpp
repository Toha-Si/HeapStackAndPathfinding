
class Input
{
    public:
        bool isDragging = false;
        double lastMouseX = 0.0, lastMouseY = 0.0;

        void BindCallbacks(GLFWwindow* window);

    private:
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};