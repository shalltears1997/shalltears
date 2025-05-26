# 実行ファイルを生成する；ファイル名は現在のディレクトリ名
CC := g++ -std=c++11
CFLAGS := -w -I./include -MMD -MP -g
#CFLAGS := -w -I./include -MMD -MP -O3

# 下はMac用
#LDFLAGS := -framework GLUT -framework OpenGL -lpthread `pkg-config --cflags --libs opencv`

# 下はUbuntu用
LDFLAGS := -lglut -lGLU -lGL -lm -lpthread `pkg-config --cflags --libs opencv`

OBJDIR := ./obj
SRCDIR := ./src
SRCS := $(notdir $(wildcard $(SRCDIR)/*.cpp))
OBJS := $(addprefix $(OBJDIR)/,$(SRCS:.cpp=.o))
DEPS := $(addprefix $(OBJDIR)/,$(SRCS:.cpp=.d))#　$(SRCS:%.c=%.d)
TARGET := $(notdir $(CURDIR))

# 標準動作開始
all: $(TARGET)

-include $(DEPS)

# o -> exe : オブジェクトファイルから実行ファイルを作成する
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# rm -f : オブジェクトファイルと実行ファイルを消去する
clean:
	$(RM) $(OBJDIR)/*~ $(OBJDIR)/*.o $(TARGET)

# cpp -> o : コードからオブジェクトファイルを作る
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<
