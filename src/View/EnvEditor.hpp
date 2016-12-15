/*
 * Copyright (C) 2010-2011 Daniel Richter <danielrichter2007@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <map>
#include <functional>


namespace Gc { namespace View { class EnvEditor :
	public Gc::Model::Logger::Trait::LoggerAware,
	public Gc::Model::Device::DeviceDataListInterfaceConnection
{
	public: virtual inline ~EnvEditor() {};

	public: std::function<void (std::string const& submountpoint)> onMountSubmountpointClick;
	public: std::function<void (std::string const& submountpoint)> onUmountSubmountpointClick;
	public: std::function<void (std::string const& newPartition)> onSwitchPartition;
	public: std::function<void (int newTypeIndex)> onSwitchBootloaderType;
	public: std::function<void ()> onOptionChange;
	public: std::function<void (bool saveConfig)> onApplyClick;
	public: std::function<void ()> onExitClick;

	public: enum class MountExceptionType {
		MOUNT_FAILED,
		UMOUNT_FAILED,
		MOUNT_ERR_NO_FSTAB,
		SUB_MOUNT_FAILED,
		SUB_UMOUNT_FAILED
	};
	public: virtual void show(bool resetPartitionChooser = false) = 0;
	public: virtual void hide() = 0;
	public: virtual void removeAllSubmountpoints() = 0;
	public: virtual void addSubmountpoint(std::string const& name, bool isActive) = 0;
	public: virtual void setEnvSettings(std::map<std::string, std::string> const& props, std::list<std::string> const& requiredProps, std::list<std::string> const& validProps) = 0;
	public: virtual std::map<std::string, std::string> getEnvSettings() = 0;
	public: virtual void setRootDeviceName(std::string const& rootDeviceName) = 0;
	public: virtual int getBootloaderType() const = 0;
	public: virtual void setSubmountpointSelectionState(std::string const& submountpoint, bool new_isSelected) = 0;
	public: virtual void showErrorMessage(MountExceptionType type)=0;
};}}

