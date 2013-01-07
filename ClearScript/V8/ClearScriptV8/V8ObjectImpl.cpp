﻿// 
// Copyright © Microsoft Corporation. All rights reserved.
// 
// Microsoft Public License (MS-PL)
// 
// This license governs use of the accompanying software. If you use the
// software, you accept this license. If you do not accept the license, do not
// use the software.
// 
// 1. Definitions
// 
//   The terms "reproduce," "reproduction," "derivative works," and
//   "distribution" have the same meaning here as under U.S. copyright law. A
//   "contribution" is the original software, or any additions or changes to
//   the software. A "contributor" is any person that distributes its
//   contribution under this license. "Licensed patents" are a contributor's
//   patent claims that read directly on its contribution.
// 
// 2. Grant of Rights
// 
//   (A) Copyright Grant- Subject to the terms of this license, including the
//       license conditions and limitations in section 3, each contributor
//       grants you a non-exclusive, worldwide, royalty-free copyright license
//       to reproduce its contribution, prepare derivative works of its
//       contribution, and distribute its contribution or any derivative works
//       that you create.
// 
//   (B) Patent Grant- Subject to the terms of this license, including the
//       license conditions and limitations in section 3, each contributor
//       grants you a non-exclusive, worldwide, royalty-free license under its
//       licensed patents to make, have made, use, sell, offer for sale,
//       import, and/or otherwise dispose of its contribution in the software
//       or derivative works of the contribution in the software.
// 
// 3. Conditions and Limitations
// 
//   (A) No Trademark License- This license does not grant you rights to use
//       any contributors' name, logo, or trademarks.
// 
//   (B) If you bring a patent claim against any contributor over patents that
//       you claim are infringed by the software, your patent license from such
//       contributor to the software ends automatically.
// 
//   (C) If you distribute any portion of the software, you must retain all
//       copyright, patent, trademark, and attribution notices that are present
//       in the software.
// 
//   (D) If you distribute any portion of the software in source code form, you
//       may do so only under this license by including a complete copy of this
//       license with your distribution. If you distribute any portion of the
//       software in compiled or object code form, you may only do so under a
//       license that complies with this license.
// 
//   (E) The software is licensed "as-is." You bear the risk of using it. The
//       contributors give no express warranties, guarantees or conditions. You
//       may have additional consumer rights under your local laws which this
//       license cannot change. To the extent permitted under your local laws,
//       the contributors exclude the implied warranties of merchantability,
//       fitness for a particular purpose and non-infringement.
//       

#include "ClearScriptV8Managed.h"

namespace Microsoft {
namespace ClearScript {
namespace V8 {

    //-------------------------------------------------------------------------
    // V8ObjectImpl implementation
    //-------------------------------------------------------------------------

    V8ObjectImpl::V8ObjectImpl(V8ObjectHolder* pHolder):
        m_gcLock(gcnew Object),
        m_pHolderPtr(new SharedPtr<V8ObjectHolder>(pHolder))
    {
    }

    //-------------------------------------------------------------------------

    SharedPtr<V8ObjectHolder> V8ObjectImpl::GetHolder()
    {
        BEGIN_LOCK_SCOPE(m_gcLock)

            if (m_pHolderPtr == nullptr)
            {
                throw gcnew ObjectDisposedException(ToString());
            }

            return *m_pHolderPtr;

        END_LOCK_SCOPE
    }

    //-------------------------------------------------------------------------

    Object^ V8ObjectImpl::GetProperty(String^ gcName)
    {
        try
        {
            return V8ProxyImpl::ExportValue(V8ObjectHelpers::GetProperty(GetHolder().GetRawPtr(), StringToUniPtr(gcName)));
        }
        catch (const V8Exception& exception)
        {
            throw gcnew InvalidOperationException(gcnew String(exception.GetMessage()));
        }
    }

    //-------------------------------------------------------------------------

    void V8ObjectImpl::SetProperty(String^ gcName, Object^ gcValue)
    {
        try
        {
            V8ObjectHelpers::SetProperty(GetHolder().GetRawPtr(), StringToUniPtr(gcName), V8ProxyImpl::ImportValue(gcValue));
        }
        catch (const V8Exception& exception)
        {
            throw gcnew InvalidOperationException(gcnew String(exception.GetMessage()));
        }
    }

    //-------------------------------------------------------------------------

    bool V8ObjectImpl::DeleteProperty(String^ gcName)
    {
        try
        {
            return V8ObjectHelpers::DeleteProperty(GetHolder().GetRawPtr(), StringToUniPtr(gcName));
        }
        catch (const V8Exception& exception)
        {
            throw gcnew InvalidOperationException(gcnew String(exception.GetMessage()));
        }
    }

    //-------------------------------------------------------------------------

    array<String^>^ V8ObjectImpl::GetPropertyNames()
    {
        try
        {
            vector<wstring> names;
            V8ObjectHelpers::GetPropertyNames(GetHolder().GetRawPtr(), names);
            auto nameCount = (int)names.size();

            auto gcNames = gcnew array<String^>(nameCount);
            for (auto index = 0; index < nameCount; index++)
            {
                gcNames[index] = gcnew String(names[index].c_str());
            }

            return gcNames;
        }
        catch (const V8Exception& exception)
        {
            throw gcnew InvalidOperationException(gcnew String(exception.GetMessage()));
        }
    }

    //-------------------------------------------------------------------------

    Object^ V8ObjectImpl::GetProperty(int index)
    {
        try
        {
            return V8ProxyImpl::ExportValue(V8ObjectHelpers::GetProperty(GetHolder().GetRawPtr(), index));
        }
        catch (const V8Exception& exception)
        {
            throw gcnew InvalidOperationException(gcnew String(exception.GetMessage()));
        }
    }

    //-------------------------------------------------------------------------

    void V8ObjectImpl::SetProperty(int index, Object^ gcValue)
    {
        try
        {
            V8ObjectHelpers::SetProperty(GetHolder().GetRawPtr(), index, V8ProxyImpl::ImportValue(gcValue));
        }
        catch (const V8Exception& exception)
        {
            throw gcnew InvalidOperationException(gcnew String(exception.GetMessage()));
        }
    }

    //-------------------------------------------------------------------------

    bool V8ObjectImpl::DeleteProperty(int index)
    {
        try
        {
            return V8ObjectHelpers::DeleteProperty(GetHolder().GetRawPtr(), index);
        }
        catch (const V8Exception& exception)
        {
            throw gcnew InvalidOperationException(gcnew String(exception.GetMessage()));
        }
    }

    //-------------------------------------------------------------------------

    array<int>^ V8ObjectImpl::GetPropertyIndices()
    {
        try
        {
            vector<int> indices;
            V8ObjectHelpers::GetPropertyIndices(GetHolder().GetRawPtr(), indices);
            auto indexCount = (int)indices.size();

            auto gcIndices = gcnew array<int>(indexCount);
            for (auto index = 0; index < indexCount; index++)
            {
                gcIndices[index] = indices[index];
            }

            return gcIndices;
        }
        catch (const V8Exception& exception)
        {
            throw gcnew InvalidOperationException(gcnew String(exception.GetMessage()));
        }
    }

    //-------------------------------------------------------------------------

    Object^ V8ObjectImpl::Invoke(array<Object^>^ gcArgs, bool asConstructor)
    {
        try
        {
            vector<V8Value> importedArgs;
            ImportValues(gcArgs, importedArgs);

            return V8ProxyImpl::ExportValue(V8ObjectHelpers::Invoke(GetHolder().GetRawPtr(), importedArgs, asConstructor));
        }
        catch (const V8Exception& exception)
        {
            throw gcnew InvalidOperationException(gcnew String(exception.GetMessage()));
        }
    }

    //-------------------------------------------------------------------------

    Object^ V8ObjectImpl::InvokeMethod(String^ gcName, array<Object^>^ gcArgs)
    {
        try
        {
            vector<V8Value> importedArgs;
            ImportValues(gcArgs, importedArgs);

            return V8ProxyImpl::ExportValue(V8ObjectHelpers::InvokeMethod(GetHolder().GetRawPtr(), StringToUniPtr(gcName), importedArgs));
        }
        catch (const V8Exception& exception)
        {
            throw gcnew InvalidOperationException(gcnew String(exception.GetMessage()));
        }
    }

    //-------------------------------------------------------------------------

    V8ObjectImpl::~V8ObjectImpl()
    {
        SharedPtr<V8ObjectHolder> psHolder;

        BEGIN_LOCK_SCOPE(m_gcLock)

            if (m_pHolderPtr != nullptr)
            {
                // hold V8 context for destruction outside lock scope
                psHolder = *m_pHolderPtr;
                delete m_pHolderPtr;
                m_pHolderPtr = nullptr;
            }

        END_LOCK_SCOPE
    }

    //-------------------------------------------------------------------------

    V8ObjectImpl::!V8ObjectImpl()
    {
        if (m_pHolderPtr != nullptr)
        {
            delete m_pHolderPtr;
        }
    }

    //-------------------------------------------------------------------------

    void V8ObjectImpl::ImportValues(array<Object^>^ gcValues, vector<V8Value>& importedValues)
    {
        importedValues.clear();
        if (gcValues != nullptr)
        {
            auto valueCount = gcValues->Length;
            importedValues.reserve(valueCount);

            for (auto index = 0; index < valueCount; index++)
            {
                importedValues.push_back(V8ProxyImpl::ImportValue(gcValues[index]));
            }
        }
    }

}}}