/*
 * Copyright (C) 2007 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef SQLStatment_h
#define SQLStatment_h

#include "PlatformString.h"
#include "Threading.h"

#include "SQLError.h"
#include "SQLResultSet.h"
#include "SQLStatementCallback.h"
#include "SQLStatementErrorCallback.h"
#include "SQLValue.h"

#include <wtf/PassRefPtr.h>
#include <wtf/RefPtr.h>
#include <wtf/Vector.h>

namespace WebCore {

class Database;
class SQLTransaction;
class String;

class SQLStatement : public ThreadSafeShared<SQLStatement> {
public:
    SQLStatement(const String& statement, const Vector<SQLValue>& arguments, PassRefPtr<SQLStatementCallback> callback, PassRefPtr<SQLStatementErrorCallback> errorCallback);
    
    bool execute(Database*);
    bool lastExecutionFailedDueToQuota() const;
    
    bool hasStatementCallback() const { return m_statementCallback; }
    bool hasStatementErrorCallback() const { return m_statementErrorCallback; }

    void setDatabaseDeletedError();
    void setVersionMismatchedError();

    bool performCallback(SQLTransaction*);
    
    SQLError* sqlError() const { return m_error.get(); }
private:
    void setFailureDueToQuota();
    void clearFailureDueToQuota();
    
    String m_statement;
    Vector<SQLValue> m_arguments;
    RefPtr<SQLStatementCallback> m_statementCallback;
    RefPtr<SQLStatementErrorCallback> m_statementErrorCallback;
    
    RefPtr<SQLError> m_error;
    RefPtr<SQLResultSet> m_resultSet;
};

} // namespace WebCore

#endif // SQLStatment_h
