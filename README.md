<h1>TrIdEnT shell 1.0 [TrIdEnT]</h1>

<p>created by : Abhishek Kumar Singh</p>

<h2>Introduction</h2>

<p>TrIdEnT shell 1.0 is a basic shell implementation. </p>

<h2>Library dependency</h2>

<p>TrIdEnT shell 1.0 depends on readline library.</p>

<h2>Compatibility</h2>

<p>TrIdEnT shell 1.0 is compatible with Linux and Mac OSx.</p>

<h2>Features</h2>

<ul>
<li>Displays a command prompt &quot;[user@host] path$ &quot;.</li>
<li>If terminal supports color then it colorizes the output.</li>
<li>Supports colorization of output to commands such as ls, grep, fgrep, egrep.</li>
<li>Supports history features(&#39;history&#39; command to show all the previously typed commands, &#39;history -c&#39; command to delete the entire history list).</li>
<li>Supports shell built-in commands and external commands.</li>
<li>Quit/Exit the shell using exit/quit commands or by pressing Ctrl + C.</li>
<li>Provides help command to see a brief idea about shell built-in commands.<br></li>
<li>Supports Tab auto-completion feature.</li>
<li>Supports setting and listing of environment variables through setenv/listenv commands.</li>
<li>Supports browsing previous commands and their modification through arrow keys.</li>
<li>Can put a process in background by using &#39;&amp;&#39; also supports piping and redirections.</li>
<li>When a Process is running and Ctrl + C is pressed then it returns to the TrIdEnT shell.</li>
</ul>

<h2>Installation</h2>

<p>Installation executable trident file (more comfortable as doesn&#39;t require installation of readline library):<br></p>

<p>Step1: copy executable file trident to /bin directory.<br></p>

<p>Step2: use trident command in terminal to invoke TrIdEnT shell.<br></p>

<p>Installing TrIdEnT shell after installing readline library:<br></p>

<p>Install readline library and then follow the following commands:<br>
Step1: make -f Makefile<br>
step2: ./trident <br></p>
