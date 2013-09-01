dofile("../glsdk/links.lua")

solution "particle"
	location ("build/")
	configurations {"Debug", "Release"}
	defines {"WIN32"}

	project "particle"
		defines {"BOOST_USER_DEFINED"}
		debugdir "working"
		kind "ConsoleApp"
		language "c++"
		files {"src/**.cpp", "include/**.h", "working/shaders/**" }
		links { "opengl32" }
		UseLibs {"glload", "glfw", "glimage", "glm", "glutil", "glmesh"}
		objdir "obj"
		includedirs {"include"}

		configuration "Debug"
			targetsuffix "D"
			defines "_DEBUG"
			flags "Symbols"
			targetdir "bin/debug"
			postbuildcommands { "copy ..\\bin\\debug\\particleD.exe ..\\working\\particle.exe" }
		
		configuration "Release"
			defines "NDEBUG"
			targetdir "bin/release"
			flags {"OptimizeSpeed", "NoFramePointer", "ExtraWarnings", "NoEditAndContinue"}
			postbuildcommands { "copy ..\\bin\\release\\particle.exe ..\\working\\particle.exe" }
