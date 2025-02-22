#
# @brief
# This file is part of the MeetiX Operating System.
# Copyright (c) 2017-2021, Marco Cicognani (marco.cicognani@meetixos.org)
#
# @developers
# Marco Cicognani (marco.cicognani@meetixos.org)
#
# @license
# GNU General Public License version 3
#

add_library(LibApi
        STATIC
        s_write_tls_master_for_process.cc
        s_spawn.cc
        s_get_pid.cc
        s_create_empty_process.cc
        s_write.cc
        s_wait_for_irq.cc
        s_configure_process.cc
        s_get_message_tx_id.cc
        s_halt_machine.cc
        s_task_get_id.cc
        s_get_created_process_id.cc
        s_receive_message.cc
        s_set_video_log.cc
        s_length.cc
        s_get_thread_ids.cc
        s_lower_malloc.cc
        s_exit.cc
        s_get_tid.cc
        s_atomic_try_lock.cc
        s_register_signal_handler.cc
        s_set_working_directory.cc
        __atomic_lock.cc
        s_register_as_server.cc
        s_cli_args_store.cc
        s_atomic_block.cc
        s_get_executable_path.cc
        s_get_task_identifier.cc
        s_attach_created_process.cc
        s_seek.cc
        s_atomic_lock.cc
        s_open_directory.cc
        s_task_register_id.cc
        s_server_manage.cc
        s_alloc_mem.cc
        s_register_irq_handler.cc
        s_get_process_ids.cc
        s_set_break.cc
        s_call_vm_86.cc
        s_get_pci_device.cc
        s_get_process_count.cc
        s_get_thread_count.cc
        s_lower_free.cc
        s_tell.cc
        s_restore_interrupted_state.cc
        s_yield.cc
        s_read.cc
        s_close.cc
        s_close_directory.cc
        s_millis.cc
        s_raise_signal.cc
        s_create_thread.cc
        s_fork.cc
        s_map_mmio.cc
        s_cancel_process_creation.cc
        s_pipe.cc
        s_fs_set_transaction_status.cc
        s_get_thread_descriptor.cc
        s_sleep.cc
        s_join.cc
        s_get_working_directory.cc
        s_log.cc
        s_fs_register_as_delegate.cc
        s_system_info.cc
        s_fs_create_node.cc
        s_read_directory.cc
        s_get_pci_device_count.cc
        s_open.cc
        __internal.cc
        s_clone_fd.cc
        s_get_process_descriptor.cc
        s_send_message.cc
        s_unmap_mem.cc
        s_kill.cc
        s_kernel_name.cc
        s_create_pages_in_spaces.cc
        s_share_mem.cc
        s_get_pid_for_tid.cc
        s_cli_args_release.cc
        do_syscall.cc s_get_date_time.cc)
add_custom_command(
        TARGET LibApi
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:LibApi> ${TOOLCHAIN_LIB}/libapi.a
)
