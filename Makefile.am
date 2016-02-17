AUTOMAKE_OPTIONS = foreign subdir-objects
INCLUDE = -I ./inc
bin_PROGRAMS = sys_server
sys_server_SOURCES = sys.c \
			src/sys_dev_manage.c \
			src/sys_net_event.c \
			src/sys_event.c \
			src/sys_serial_event.c \
			src/sys_event_dispatch.c \
			src/sys_bt_event.c \
			src/threadpool.c \
			src/sys_task.c

#LIBADD = @PTHREAD_LIBS@

