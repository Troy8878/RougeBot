# This file is just a sample that gives you an idea of the interface
# for the camera wrapper classes. The actual implementation is C++.
# This file is not evaluated in the engine.

module Cameras
  def Cameras.find_camera(name)
    # Retrieves the wrapper for the given camera name
  end

  class Camera
  end

  class HUDCamera < Camera
    def position
      # gets the vector for the camera position
    end

    def size
      # gets the scale of this camera (default 1)
    end

    def size=(value)
      # sets the scale of this camera
    end
  end

  class LookAtCamera < Camera
    def position
      # gets the vector for the camera position
    end

    def look_at
      # gets the vector for the camera look position
    end

    def fov
      # gets the field of view for this camera
    end

    def fov=(value)
      # sets the field of view for this camera
    end
  end
end
