/*==================================================================================
    Copyright (c) 2008, binaryzebra
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the binaryzebra nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE binaryzebra AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL binaryzebra BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=====================================================================================*/



#include "TextureDescriptorTool.h"
#include "TextureDescriptorUtils.h"

#include "TexturePacker/CommandLineParser.h"

using namespace DAVA;


void TextureDescriptorTool::PrintUsage()
{
    printf("\n");
    printf("-texdescriptor [-resave] [-copycompression] [-create] [-folder [folder for action with descriptors]]\n");
	printf("-texdescriptor [-setcompression] [-file [descriptors folderPathname]] [-folder [folder with descriptors]] [-gpuName gpuValue] [-f] [-convert]\n");
    printf("\tDo different operations with *.tex files\n");
    printf("\t-resave - resave all *.tex with new format\n");
    printf("\t-copycompression - copy compressionParams parameters from PowerVR_iOS to other gpus\n");
    printf("\t-create - create *.tex for *.png if need\n");
	printf("\t-setcompression - set compressionParams parameters for *tex or for all *text in folder. -f enables force mode. -convert runs convertation to selected format\n");
    printf("\t-changecompression - change compressionParams parameters for *tex. -convert runs convertation to selected format\n");

    
    printf("\n");
    printf("Samples:\n");
    printf("-texdescriptor -resave -folder /Users/User/Project/DataSource/3d/\n");
    printf("-texdescriptor -copycompression -folder /Users/User/Project/DataSource/3d/\n");
    printf("-texdescriptor -create -folder /Users/User/Project/DataSource/3d/\n");
	printf("-texdescriptor -setcompression -file /Users/User/Project/DataSource/3d/Tanks/images/a-20.tex -PowerVR_iOS PVR4 -tegra DXT1 -mali ETC1 -adreno RGBA4444\n");
	printf("-texdescriptor -setcompression -folder /Users/User/Project/DataSource/3d/Tanks/images/ -PowerVR_iOS PVR4 -tegra DXT1 -mali ETC1 -adreno RGBA4444 -f -convert\n");
    printf("-texdescriptor -changecompression -file /Users/User/Project/DataSource/3d/Tanks/images/1.tex -tegra -sf ETC1 -tf DXT1 -f -convert\n");
}

DAVA::String TextureDescriptorTool::GetCommandLineKey()
{
    return "-texdescriptor";
}

bool TextureDescriptorTool::InitializeFromCommandLine()
{
    commandAction = ACTION_NONE;
    
    if(CommandLineParser::CommandIsFound("-resave"))
    {
        commandAction = ACTION_RESAVE_DESCRIPTORS;
		folderPathname = ReadFolderPathname();
    }
    else if(CommandLineParser::CommandIsFound("-copycompression"))
    {
        commandAction = ACTION_COPY_COMPRESSION;
		folderPathname = ReadFolderPathname();
    }
    else if(CommandLineParser::CommandIsFound("-create"))
    {
        commandAction = ACTION_CREATE_DESCRIPTORS;
		folderPathname = ReadFolderPathname();
    }
    else if(CommandLineParser::CommandIsFound("-changecompression"))
    {
		filePathname = ReadFilePathname();
        if(!filePathname.IsEmpty())
		{
			commandAction = ACTION_CHANGE_COMPRESSION_FOR_DESCRIPTOR;
		}
		else
		{
			errors.insert("[TextureDescriptorTool]: path not set");
			return false;
		}
        
        forceModeEnabled = CommandLineParser::CommandIsFound("-f");
		convertEnabled = CommandLineParser::CommandIsFound("-convert");
        
        ReadCompressionChangeParams();
    }
	else if(CommandLineParser::CommandIsFound("-setcompression"))
	{
		folderPathname = ReadFolderPathname();
		filePathname = ReadFilePathname();

		if(!folderPathname.IsEmpty())
		{
			commandAction = ACTION_SET_COMPRESSION_FOR_FOLDER;
		}
		else if(!filePathname.IsEmpty())
		{
			commandAction = ACTION_SET_COMPRESSION_FOR_DESCRIPTOR;
		}
		else
		{
			errors.insert("[TextureDescriptorTool]: path not set");
			return false;
		}

		forceModeEnabled = CommandLineParser::CommandIsFound("-f");
		convertEnabled = CommandLineParser::CommandIsFound("-convert");

		ReadCompressionParams();
	}
    else
    {
        errors.insert("[TextureDescriptorTool]: action not set");
        return false;
    }
    
    return true;
}

void TextureDescriptorTool::ReadCompressionChangeParams()
{
    for(int32 i = GPU_POWERVR_IOS; i < GPU_FAMILY_COUNT; ++i)
	{
		eGPUFamily gpu = (eGPUFamily)i;
		String gpuFlag = "-" + GPUFamilyDescriptor::GetGPUName(gpu);
		if(CommandLineParser::CommandIsFound(gpuFlag))
		{
            TextureDescriptor::Compression sourceCompression, textureCompression;
            if(CommandLineParser::CommandIsFound("-sf"))
            {
                String sourceFormatName = CommandLineParser::GetCommandParam(String("-sf"));
                sourceCompression.format = Texture::GetPixelFormatByName(sourceFormatName);
                sourceCompression.compressToWidth = sourceCompression.compressToHeight = 0;
            }
            if(CommandLineParser::CommandIsFound("-tf"))
            {
                String textureFormatName = CommandLineParser::GetCommandParam(String("-tf"));
                textureCompression.format = Texture::GetPixelFormatByName(textureFormatName);
                textureCompression.compressToWidth = textureCompression.compressToHeight = 0;
            }
            
            compressionChangeParams[gpu] = std::pair<DAVA::TextureDescriptor::Compression,
                                            DAVA::TextureDescriptor::Compression>(sourceCompression, textureCompression);
		}
	}
}

void TextureDescriptorTool::ReadCompressionParams()
{
	compressionParams.clear();
	for(int32 i = GPU_POWERVR_IOS; i < GPU_FAMILY_COUNT; ++i)
	{
		eGPUFamily gpu = (eGPUFamily)i;
		String gpuFlag = "-" + GPUFamilyDescriptor::GetGPUName(gpu);
		if(CommandLineParser::CommandIsFound(gpuFlag))
		{
			String formatName = CommandLineParser::GetCommandParam(gpuFlag);

			TextureDescriptor::Compression compression;
			compression.format = Texture::GetPixelFormatByName(formatName);
			compression.compressToWidth = compression.compressToHeight = 0;


			String widthStr = CommandLineParser::GetCommandParamAdditional(gpuFlag, 1);
			String heightStr = CommandLineParser::GetCommandParamAdditional(gpuFlag, 2);

			if(!widthStr.empty() && !heightStr.empty())
			{
				compression.compressToWidth = atoi(widthStr.c_str());
				compression.compressToHeight = atoi(heightStr.c_str());

				if(compression.compressToWidth < 0 || compression.compressToHeight < 0)
				{
					Logger::Error("[TextureDescriptorTool::ReadCompressionParams] Wrong size parameters for flag: %s", gpuFlag.c_str());
					compression.compressToWidth = compression.compressToHeight = 0;
				}
			}

			compressionParams[gpu] = compression;
		}
	}
}


void TextureDescriptorTool::Process()
{
    switch(commandAction)
    {
        case ACTION_RESAVE_DESCRIPTORS:
            TextureDescriptorUtils::ResaveDescriptorsForFolder(folderPathname);
            break;
            
        case ACTION_COPY_COMPRESSION:
            TextureDescriptorUtils::CopyCompressionParamsForFolder(folderPathname);
            break;
            
        case ACTION_CREATE_DESCRIPTORS:
            TextureDescriptorUtils::CreateDescriptorsForFolder(folderPathname);
			break;

		case ACTION_SET_COMPRESSION_FOR_FOLDER:
			TextureDescriptorUtils::SetCompressionParamsForFolder(folderPathname, compressionParams, convertEnabled, forceModeEnabled);
			break;

		case ACTION_SET_COMPRESSION_FOR_DESCRIPTOR:
			TextureDescriptorUtils::SetCompressionParams(filePathname, compressionParams, convertEnabled, forceModeEnabled);
			break;
        case ACTION_CHANGE_COMPRESSION_FOR_DESCRIPTOR:
            TextureDescriptorUtils::ChangeCompressionParams(filePathname, compressionChangeParams, convertEnabled, forceModeEnabled);
            break;

        default:
            Logger::Error("[TextureDescriptorTool::Process] Unhandled action!");
			break;
    }
}

DAVA::FilePath TextureDescriptorTool::ReadFolderPathname() const
{
	FilePath folder = CommandLineParser::GetCommandParam(String("-folder"));
	if(!folder.IsEmpty())
	{
		folder.MakeDirectoryPathname();
		return folder;
	}

	return FilePath();
}

DAVA::FilePath TextureDescriptorTool::ReadFilePathname() const
{
	return CommandLineParser::GetCommandParam(String("-file"));
}


