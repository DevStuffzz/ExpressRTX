-- premake5.lua
workspace "ExpressRTX"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "ExpressRTX"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "ExpressRTX"