# TEA_SRC as environment variable

CC = gcc
INCLUDE = -Ilib
# -fPIC?
CFLAGS = -Wall -O0 -g $(INCLUDE) -DTEA_BUILD_AS_DLL
LDFLAGS = -shared -g
LIBS = -L. ./libffi.a -ltea00
# -lffi?

SRC = $(wildcard src/*.c)
OBJS = $(SRC:.c=.o)

TARGET = ffi.dll
STRIP = strip --strip-unneeded

Q = @
E = @echo

all: $(TARGET)

$(TARGET): $(OBJS)
	$(E) "LINK          $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(E) "STRIP         $@"
	$(Q)$(STRIP) $@
	$(E) "MOVE          $@  ->  src/lib"
	$(Q) mv $(TARGET) "$(TEA_SRC)/lib/"

src/%.o: src/%.c
	$(E) "CC            $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<