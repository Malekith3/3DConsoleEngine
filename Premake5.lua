workspace "Console3DEngine"
	architecture "x64"
	configurations 
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "3DEngine"
	location "3DEngine"
	kind "ConsoleApp"
	language "C++"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")

	 files 
	 { 
	 	"%{prj.name}/source/Headers/*.h",
	 	"%{prj.name}/source/*.cpp",
	 }

	 includedirs
	 {
		"%{prj.name}/source"
	 }

	filter "system:windows"
	  cppdialect "C++17"
	  staticruntime "On"
	  systemversion "latest"
      defines { "_CRT_SECURE_NO_WARNINGS" }
      symbols "On"

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols  "On"
	  optimize "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      symbols  "On"
	  optimize "On"

