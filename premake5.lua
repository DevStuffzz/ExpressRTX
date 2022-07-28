-- premake5.lua
workspace "RTX"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "RTX"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "RTX"