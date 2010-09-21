/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2000-2009 Kitware, Inc., Insight Software Consortium

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/

#include "cmCPackTarBZip2Generator.h"
//----------------------------------------------------------------------
cmCPackTarBZip2Generator::cmCPackTarBZip2Generator() 
 :cmCPackArchiveGenerator(cmCPackArchiveGenerator::BZIP2,
                          cmCPackArchiveGenerator::TAR)
{
}

//----------------------------------------------------------------------
cmCPackTarBZip2Generator::~cmCPackTarBZip2Generator()
{
}

