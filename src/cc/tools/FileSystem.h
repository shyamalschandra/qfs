//---------------------------------------------------------- -*- Mode: C++ -*-
// $Id$
//
// Created 2012/09/11
// Author: Mike Ovsiannikov
//
// Copyright 2012,2016 Quantcast Corporation. All rights reserved.
//
// This file is part of Kosmos File System (KFS).
//
// Licensed under the Apache License, Version 2.0
// (the "License"); you may not use this file except in compliance with
// the License. You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied. See the License for the specific language governing
// permissions and limitations under the License.
//
// \brief Generic file system interface.
//
//----------------------------------------------------------------------------

#ifndef TOOLS_FILE_SYSTEM_H
#define TOOLS_FILE_SYSTEM_H

#include "common/kfstypes.h"
#include "libclient/KfsClient.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glob.h>
#include <inttypes.h>

#include <string>

namespace KFS {
class Properties;

namespace tools {

using std::string;

class FileSystem
{
public:
    typedef KfsClient::ErrorHandler ErrorHandler;
    typedef vector<vector<string> > DataLocations;

    class StatBuf : public stat
    {
    public:
        StatBuf()
            : stat(),
              mSubCount1(-1),
              mSubCount2(-1),
              mNumReplicas(-1),
              mNumStripes(-1),
              mNumRecoveryStripes(-1),
              mStriperType(-1),
              mStripeSize(-1),
              mMinSTier(-1),
              mMaxSTier(-1)
            {}
        void Reset()
            { *this = StatBuf(); }
        int64_t mSubCount1; /// chunk in the file or files in directory count
        int64_t mSubCount2; /// directories count, or next chunk position
        int16_t mNumReplicas;
        int16_t mNumStripes;
        int16_t mNumRecoveryStripes;
        int32_t mStriperType;
        int32_t mStripeSize;
        int16_t mMinSTier;
        int16_t mMaxSTier;
    };
    class DirIterator
    {
    protected:
        DirIterator()
            {}
        virtual ~DirIterator()
            {}
    };
    static int SetDefault(
        const string&     inUri,
        const Properties* inPropertiesPtr = 0);
    static int Get(
        const string&     inUri,
        FileSystem*&      outFsPtr,
        string*           outPathPtr      = 0,
        const Properties* inPropertiesPtr = 0);
    static string GetStrError(
        int               inErr,
        const FileSystem* inFsPtr = 0);
    virtual int Chdir(
        const string& inDir) = 0;
    virtual int GetCwd(
        string& outDir) = 0;
    virtual int Open(
        const string& inFileName,
        int           inFlags,
        int           inMode,
        const string* inParamsPtr) = 0;
    int Open(
        const string& inFileName,
        int           inFlags,
        int           inMode)
        { return Open(inFileName, inFlags, inMode, 0); }
    virtual int Close(
        int inFd) = 0;
    virtual ssize_t Read(
        int    inFd,
        void*  inBufPtr,
        size_t inBufSize) = 0;
    virtual ssize_t Write(
        int          inFd,
        const void*  inBufPtr,
        size_t       inBufSize) = 0;
    virtual int Sync(
        int inFd) = 0;
    virtual int64_t Seek(
        int     inFd,
        int64_t inOffset,
        int     inWhence) = 0;
    virtual int Stat(
        const string& inFileName,
        StatBuf&      outStat) = 0;
    virtual int Stat(
        int      inFd,
        StatBuf& outStat) = 0;
    virtual int Open(
        const string& inDirName,
        bool          inFetchAttributesFlag,
        DirIterator*& outDirIteratorPtr) = 0;
    virtual int Close(
        DirIterator* inDirIteratorPtr) = 0;
    virtual int Next(
        DirIterator*    inDirIteratorPtr,
        string&         outName,
        const StatBuf*& outStatPtr) = 0;
    virtual int Glob(
        const string& inPattern,
        int           inFlags,
        int (*inErrFuncPtr) (const char* inErrPathPtr, int inErrno),
        glob_t*        inGlobPtr) = 0;
    virtual int Chmod(
        const string& inPathName,
        kfsMode_t     inMode,
        bool          inRecursiveFlag,
        ErrorHandler* inErrorHandlerPtr) = 0;
    virtual int Chown(
        const string& inPathName,
        const char*   inOwnerNamePtr,
        const char*   inGroupNamePtr,
        bool          inRecursiveFlag,
        ErrorHandler* inErrorHandlerPtr) = 0;
    virtual int Rmdir(
        const string& inPathName) = 0;
    virtual int Remove(
        const string& inPathName,
        bool          inRecursiveFlag,
        ErrorHandler* inErrorHandlerPtr) = 0;
    virtual int Mkdir(
        const string& inPathName,
        kfsMode_t     inMode,
        bool          inCreateAllFlag) = 0;
    virtual int Rename(
        const string& inSrcName,
        const string& inDstName) = 0;
    virtual int SetUMask(
        mode_t inUMask) = 0;
    virtual int GetUMask(
        mode_t& outUMask) = 0;
    virtual int GetUserAndGroupNames(
        kfsUid_t inUser,
        kfsGid_t inGroup,
        string&  outUserName,
        string&  outGroupName) = 0;
    virtual int GetUserAndGroupIds(
        const string& inUserName,
        const string& inGroupName,
        kfsUid_t&     outUserId,
        kfsGid_t&     outGroupId) = 0;
    virtual int GetUserName(
        string& outUserName) = 0;
    virtual int SetMtime(
        const string&         inPath,
        const struct timeval& inMTime) = 0;
    virtual int SetUtimes(
        const string&         inPath,
        const struct timeval& inMTime,
        int64_t               inATime,
        int64_t               inCTime) = 0;
    virtual int SetReplication(
        const string& inPath,
        int           inReplication,
        bool          inRecursiveFlag,
        ErrorHandler* inErrorHandlerPtr) = 0;
    virtual int GetReplication(
        const string& inPath,
        StatBuf&      outStat,
        int&          outMinReplication,
        int&          outMaxReplication) = 0;
    virtual int SetStorageTierRange(
        const string& inPath,
        int           inMinSTier,
        int           inMaxSTier) = 0;
    virtual int GetHomeDirectory(
        string& outHomeDir) = 0;
    virtual string StrError(
        int inError) const = 0;
    virtual const string& GetUri() const = 0;
    virtual int CreateDelegationToken(
        bool      inAllowDelegationFlag,
        uint32_t  inMaxValidForSec,
        bool&     outDelegationAllowedFlag,
        uint64_t& outIssuedTime,
        uint32_t& outTokenValidForSec,
        uint32_t& outDelegationValidForSec,
        string&   outToken,
        string&   outKey,
        string*   outErrMsgPtr) = 0;
    virtual int RenewDelegation(
        string&   ioToken,
        string&   ioKey,
        bool&     outDelegationAllowedFlag,
        uint64_t& outIssuedTime,
        uint32_t& outTokenValidForSec,
        uint32_t& outDelegationValidForSec,
        string*   outErrMsgPtr) = 0;
    virtual int CancelDelegation(
        const string& inToken,
        const string& inKey,
        string*       outErrMsgPtr) = 0;
    virtual int GetDataLocation(
        const string   inPath,
        int64_t        inStartPos,
        int64_t        inLength,
        DataLocations& outLocations,
        int64_t&       outBlockSize) = 0;
    virtual int GetDelegationTokenInfo(
        const char* inTokenStrPtr,
        kfsUid_t&   outUid,
        uint32_t&   outSeq,
        kfsKeyId_t& outKeyId,
        int16_t&    outFlags,
        uint64_t&   outIssuedTime,
        uint32_t&   outValidForSec,
        string*     outErrMsgPtr) = 0;
    virtual bool operator==(
        const FileSystem& inFs) const = 0;
    bool operator!=(
        const FileSystem& inFs)
        { return ! (inFs == *this); }
protected:
    virtual ~FileSystem()
        {}
};

}
}

#endif /* TOOLS_FILE_SYSTEM_H */
