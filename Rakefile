CC = "gcc"
CPP = "g++"
GTEST_DIR=File.join(__dir__, "extsrc/googletest/googletest")
RAKE_ROOT=__dir__

execs = %w(client server)

task :default => execs

task :clean do |t|
  sh "rm -f *.o"
  sh "rm -f *.a"
  sh "rm -f *.sock"
  execs.each do |name|
    sh "rm #{name}" if(File.exists?(name))
  end
  sh "rm test/run_test" if File.exists?("test/run_test")
end

task :example => :all do |t|
  execs.each do |name|
    sh "gcc -I./include -L. ./example/#{name}.c -l#{name} -lpthread -o #{name}"
  end
end

task :test => [:default, :build_gtest] do |t|
  test_objs = %w(test_mmap_info)

  Dir.chdir("./test") do
    objs = Dir.glob(File.join(RAKE_ROOT, "*.o"))

    test_objs.each do |name|
      sh "#{CPP} -I. -I#{GTEST_DIR}/include -I#{RAKE_ROOT}/include -c #{name}.cpp #{objs.join(' ')} -o #{name}.o -lpthread"
    end

    sh "#{CPP} -I. -I#{RAKE_ROOT}/include -I#{GTEST_DIR}/include -lpthread test_main.cpp #{test_objs.map { |name| "#{name}.o" }.join(' ')} #{GTEST_DIR}/libtest.a #{objs.join(' ')} -o run_test"
    sh "./run_test"
  end
end

task :build_gtest do |t|
  Dir.chdir(GTEST_DIR) do
    sh "#{CPP} -isystem ./include -I. -pthread -c ./src/gtest-all.cc"
    sh "ar -rv libtest.a gtest-all.o"
  end
end

task :all => [:clean, :default]

execs.each do |exec|
  task exec => "lib#{exec}.a"

  file "lib#{exec}.a" => ["mmap_info.o", "ipc_base.o", "ipc_#{exec}.o"] do |t|
    sh "ar -r #{t.name} #{t.prerequisites.join(' ')}"
  end
end

rule '.o' => './src/%X.c' do |t|
  sh "gcc -I./include -c -o #{t.name} #{t.prerequisites.join(' ')} -lpthread"
end
