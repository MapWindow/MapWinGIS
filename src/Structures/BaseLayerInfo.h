#pragma once

/*
Common base class (well, struct) for ShapeLayerInfo.
It simply allows Layer dtor to easily delete its addInfo member
(using virtual dtor defined here).
*/
struct BaseLayerInfo
{
	virtual ~BaseLayerInfo() {}
};