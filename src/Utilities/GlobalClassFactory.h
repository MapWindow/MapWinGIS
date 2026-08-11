#pragma once

class GlobalClassFactory
{
public:
	IClassFactory* pointFactory;
	IClassFactory* shapeFactory;
	IClassFactory* extentsFactory;

private:
	HRESULT m_pointFactoryStatus;
	HRESULT m_shapeFactoryStatus;
	HRESULT m_extentsFactoryStatus;

public:
	GlobalClassFactory::GlobalClassFactory()
		: pointFactory(nullptr)
		, shapeFactory(nullptr)
		, extentsFactory(nullptr)
		, m_pointFactoryStatus(S_FALSE)
		, m_shapeFactoryStatus(S_FALSE)
		, m_extentsFactoryStatus(S_FALSE)
	{
		// Try to get class factories, but don't crash if they fail
		m_pointFactoryStatus = CoGetClassObject(CLSID_Point, CLSCTX_INPROC_SERVER, nullptr, IID_IClassFactory, reinterpret_cast<void**>(&pointFactory));
		if (FAILED(m_pointFactoryStatus))
		{
			pointFactory = nullptr;
		}

		m_shapeFactoryStatus = CoGetClassObject(CLSID_Shape, CLSCTX_INPROC_SERVER, nullptr, IID_IClassFactory, reinterpret_cast<void**>(&shapeFactory));
		if (FAILED(m_shapeFactoryStatus))
		{
			shapeFactory = nullptr;
		}

		m_extentsFactoryStatus = CoGetClassObject(CLSID_Extents, CLSCTX_INPROC_SERVER, nullptr, IID_IClassFactory, reinterpret_cast<void**>(&extentsFactory));
		if (FAILED(m_extentsFactoryStatus))
		{
			extentsFactory = nullptr;
		}
	}

	~GlobalClassFactory()
	{
		if (pointFactory)
		{
			pointFactory->Release();
			pointFactory = nullptr;
		}
		if (shapeFactory)
		{
			shapeFactory->Release();
			shapeFactory = nullptr;
		}
		if (extentsFactory)
		{
			extentsFactory->Release();
			extentsFactory = nullptr;
		}
	}

	// Check if all factories are available
	BOOL IsInitialized() const
	{
		return pointFactory != nullptr && shapeFactory != nullptr && extentsFactory != nullptr;
	}

	// Check if a specific factory is available
	BOOL IsPointFactoryAvailable() const
	{
		return pointFactory != nullptr;
	}

	BOOL IsShapeFactoryAvailable() const
	{
		return shapeFactory != nullptr;
	}

	BOOL IsExtentsFactoryAvailable() const
	{
		return extentsFactory != nullptr;
	}

	// Get error status for debugging
	HRESULT GetPointFactoryStatus() const
	{
		return m_pointFactoryStatus;
	}

	HRESULT GetShapeFactoryStatus() const
	{
		return m_shapeFactoryStatus;
	}

	HRESULT GetExtentsFactoryStatus() const
	{
		return m_extentsFactoryStatus;
	}
};