% File 'sab.cls' Class for SAB Proceedings
% by Stéphane Doncieux 11/10/1999


\NeedsTeXFormat{LaTeX2e}[1995/12/01]
\ProvidesClass{sab}
              [11/10/1999
 LaTeX document class for SAB Proceedings]
\newcommand\@ptsize{}
\newif\if@restonecol
\newif\if@titlepage
\@titlepagefalse
\@twocolumntrue
\@twosidetrue

\DeclareOption{a4}
{\setlength\paperheight {297mm}%
  \setlength\paperwidth  {210mm}}

\DeclareOption{letter}
{\setlength\paperheight {8.5in}%
  \setlength\paperwidth  {11in}}



\renewcommand\@ptsize{0}


\def\@affiliation{}
\newcommand \affiliation [1] {
\def\@affiliation{#1}
}


\leftmargini 15pt
\leftmarginii 22pt \leftmarginiii 18.7pt \leftmarginiv 17pt \leftmarginv 10pt
\leftmarginvi 10pt
\leftmargin\leftmargini
\labelsep 5pt
\labelwidth\leftmargini\advance\labelwidth-\labelsep
\def\@listI{\leftmargin 15pt \leftmargini 15pt
\parsep 4pt plus 2pt minus 1pt\topsep 8pt
plus 2pt minus 4pt\itemsep 4pt plus 2pt minus 1pt}
\let\@listi\@listI
\@listi
\def\@listii{\leftmargin\leftmarginii
 \labelwidth\leftmarginii\advance\labelwidth-\labelsep
 \topsep 4pt plus 2pt minus 1pt
 \parsep 2pt plus 1pt minus 1pt
 \itemsep \parsep}
\def\@listiii{\leftmargin\leftmarginiii
 \labelwidth\leftmarginiii\advance\labelwidth-\labelsep
 \topsep 2pt plus 1pt minus 1pt
 \parsep \z@ \partopsep 1pt plus 0pt minus 1pt
 \itemsep \topsep}
\def\@listiv{\leftmargin\leftmarginiv
 \labelwidth\leftmarginiv\advance\labelwidth-\labelsep}
\def\@listv{\leftmargin\leftmarginv
 \labelwidth\leftmarginv\advance\labelwidth-\labelsep}
\def\@listvi{\leftmargin\leftmarginvi
 \labelwidth\leftmarginvi\advance\labelwidth-\labelsep}
\relax


\footnotesep 6.65pt
\skip\footins 9pt plus 4pt minus 2pt
\floatsep 12pt plus 2pt minus 2pt \textfloatsep 20pt plus 2pt minus 4pt
\intextsep 12pt plus 2pt minus 2pt \dblfloatsep 12pt plus 2pt
minus 2pt \dbltextfloatsep 20pt plus 2pt minus 4pt
\@fptop 0pt plus 1fil \@fpsep 8pt plus 2fil \@fpbot 0pt plus 1fil
\@dblfptop 0pt plus 1fil \@dblfpsep 8pt plus 2fil \@dblfpbot 0pt plus 1fil
\marginparpush 5pt
 
\parskip 0pt plus 1pt \parindent 15pt \partopsep 2pt plus 1pt minus 1pt
\@lowpenalty 51 \@medpenalty 151 \@highpenalty 301
\@beginparpenalty -\@lowpenalty \@endparpenalty -\@lowpenalty \@itempenalty
-\@lowpenalty
%\ExecuteOptions{10pt,twoside,onecolumn,final,letter}
\ExecuteOptions{10pt,twoside,letter}
\ProcessOptions

\input{size1\@ptsize.clo}

\setlength\lineskip{1\p@}
\setlength\normallineskip{1\p@}
\renewcommand\baselinestretch{1}
\setlength\parskip{0\p@ \@plus \p@}
\@lowpenalty   51
\@medpenalty  151
\@highpenalty 301
\setcounter{topnumber}{2}
\renewcommand\topfraction{.7}
\setcounter{bottomnumber}{1}
\renewcommand\bottomfraction{.3}
\setcounter{totalnumber}{3}
\renewcommand\textfraction{.2}
\renewcommand\floatpagefraction{.5}
\setcounter{dbltopnumber}{2}
\renewcommand\dbltopfraction{.7}
\renewcommand\dblfloatpagefraction{.5}

% pour remplacer les [ par des ( dans les \cite de la biblio
\renewcommand{\@cite}[2]{({#1\if@tempswa , #2\fi})}
\renewcommand{\@biblabel}[1]{}
%\renewcommand{\@bibitem}[1]{#1}

\def\ps@headings{%
  \let\@oddfoot\@empty\let\@evenfoot\@empty
  \def\@evenhead{\large\bfseries\thepage\hspace{10mm}\@author\hfil}%
  \def\@oddhead{\large\bfseries\hfil\@title\hspace{10mm}\thepage}%
  \let\@mkboth\markboth
  \def\sectionmark##1{%
    \markboth {\MakeUppercase{%
        \ifnum \c@secnumdepth >\z@
        \thesection\quad
        \fi
        ##1.}}{}}%
  \def\subsectionmark##1{%
    \markright {%
      \ifnum \c@secnumdepth >\@ne
      \thesubsection\quad
      \fi
      ##1}}}
\def\ps@myheadings{%
    \let\@oddfoot\@empty\let\@evenfoot\@empty
    \def\@evenhead{\thepage\hfil\slshape\leftmark}%
    \def\@oddhead{{\slshape\rightmark}\hfil\thepage}%
    \let\@mkboth\@gobbletwo
    \let\sectionmark\@gobble
    \let\subsectionmark\@gobble
    }

\newcommand\maketitle{\par
  \begingroup
    \renewcommand\thefootnote{\@fnsymbol\c@footnote}%
    \def\@makefnmark{\rlap{\@textsuperscript{\normalfont\@thefnmark}}}%
    \long\def\@makefntext##1{\parindent 1em\noindent
            \hb@xt@1.8em{%
                \hss\@textsuperscript{\normalfont\@thefnmark}}##1}%
    \if@twocolumn
      \ifnum \col@number=\@ne
        \@maketitle
      \else
        \twocolumn[\@maketitle]%
      \fi
    \else
      \newpage
      \global\@topnum\z@   % Prevents figures from going at top of page.
      \@maketitle
    \fi
    \thispagestyle{empty}\@thanks
  \endgroup
  \setcounter{footnote}{0}%
  \global\let\thanks\relax
  \global\let\maketitle\relax
  \global\let\@maketitle\relax
  \global\let\@thanks\@empty
%  \global\let\@author\@empty
  \global\let\@date\@empty
%  \global\let\@title\@empty
  \global\let\title\relax
  \global\let\author\relax
  \global\let\date\relax
  \global\let\and\relax
}
\def\@maketitle{%
  \newpage
  \null
  %\vskip 2em%
  \begin{center}%
  \let \footnote \thanks
    {\LARGE \@title \par}%
    \vskip 1.5em%
    {\large \bfseries
      \lineskip .5em%
      \begin{tabular}[t]{c}%
        \@author 
      \end{tabular}\par}%
    {\large 
      \lineskip .5em%
      \begin{tabular}[t]{c}%
        \@affiliation
      \end{tabular}\par}%


%    \vskip 1em%
%    {\large \@date}%
  \end{center}%
  \par
  \vskip 1.5em}
%\fi

\setcounter{secnumdepth}{3}
\newcounter {part}
\newcounter {section}
\newcounter {subsection}[section]
\newcounter {subsubsection}[subsection]
\newcounter {paragraph}[subsubsection]
\newcounter {subparagraph}[paragraph]
\renewcommand \thepart {\@Roman\c@part}
\renewcommand \thesection {\@arabic\c@section.}
\renewcommand\thesubsection   {\thesection\@arabic\c@subsection}
\renewcommand\thesubsubsection{\thesubsection .\@arabic\c@subsubsection}
\renewcommand\theparagraph    {\thesubsubsection.\@arabic\c@paragraph}
\renewcommand\thesubparagraph {\theparagraph.\@arabic\c@subparagraph}
\newcommand\part{\par
   \addvspace{4ex}%
   \@afterindentfalse
   \secdef\@part\@spart}

\def\@part[#1]#2{%
    \ifnum \c@secnumdepth >\m@ne
      \refstepcounter{part}%
      \addcontentsline{toc}{part}{\thepart\hspace{1em}#1}%
    \else
      \addcontentsline{toc}{part}{#1}%
    \fi
    {\parindent \z@ \raggedright
     \interlinepenalty \@M
     \normalfont
     \ifnum \c@secnumdepth >\m@ne
       \Large\bfseries \partname~\thepart
       \par\nobreak
     \fi
     \huge \bfseries #2%
     \markboth{}{}\par}%
    \nobreak
    \vskip 3ex
    \@afterheading}

\def\@spart#1{%
    {\parindent \z@ \raggedright
     \interlinepenalty \@M
     \normalfont
     \huge \bfseries #1\par}%
     \nobreak
     \vskip 3ex
     \@afterheading}
\newcommand\section{\@startsection {section}{1}{\z@}%
                                   {-10pt}
                                   {8pt}
                                   {\large\bf}}
\newcommand\subsection{\@startsection{subsection}{2}{\z@}%
                                     {-10pt}
                                     {8pt}
                                     {\large\em}}
\newcommand\subsubsection{\@startsection{subsubsection}{3}{\z@}%
                                     {-3.25ex\@plus -1ex \@minus -.2ex}%
                                     {1.5ex \@plus .2ex}%
                                     {\large\em}}
\newcommand\paragraph{\@startsection{paragraph}{4}{\z@}%
                                    {3.25ex \@plus1ex \@minus.2ex}%
                                    {-1em}%
                                    {\normalsize\bf}}
\newcommand\subparagraph{\@startsection{subparagraph}{5}{\parindent}%
                                       {3.25ex \@plus1ex \@minus .2ex}%
                                       {-1em}%
                                      {\normalsize\bf}}






\setlength\topsep {4pt plus 2pt minus 2pt}
\setlength\parsep {2pt plus 1pt minus 1pt} 
\itemsep \parsep
\setlength  \labelsep  {.5em}
\setlength  \labelwidth{\leftmargini}
\addtolength\labelwidth{-\labelsep}




\@beginparpenalty -\@lowpenalty
\@endparpenalty   -\@lowpenalty
\@itempenalty     -\@lowpenalty
\renewcommand\theenumi{\@arabic\c@enumi}
\renewcommand\theenumii{\@alph\c@enumii}
\renewcommand\theenumiii{\@roman\c@enumiii}
\renewcommand\theenumiv{\@Alph\c@enumiv}
\newcommand\labelenumi{\theenumi.}
\newcommand\labelenumii{(\theenumii)}
\newcommand\labelenumiii{\theenumiii.}
\newcommand\labelenumiv{\theenumiv.}
\renewcommand\p@enumii{\theenumi}
\renewcommand\p@enumiii{\theenumi(\theenumii)}
\renewcommand\p@enumiv{\p@enumiii\theenumiii}

% a verifier, ne semble pas marcher
\newcommand\labelitemi{$\bullet$}
\newcommand\labelitemii{\bf --}
\newcommand\labelitemiii{$\ast$}
\newcommand\labelitemiv{$\cdot$}

\newenvironment{description}
               {\list{}{\labelwidth\z@ \itemindent-\leftmargin
                        \let\makelabel\descriptionlabel}}
               {\endlist}
\newcommand*\descriptionlabel[1]{\hspace\labelsep
                                \normalfont\bfseries #1}
\newenvironment{abstract}{%
  \begin{center}
    \bfseries\large\abstractname
  \end{center}
\quotation
  }
{\endquotation}

\newenvironment{verse}
               {\let\\\@centercr
                \list{}{\itemsep      \z@
                        \itemindent   -1.5em%
                        \listparindent\itemindent
                        \rightmargin  \leftmargin
                        \advance\leftmargin 1.5em}%
                \item\relax}
               {\endlist}

\newenvironment{quotation}
               {\list{}{\listparindent 0.75em%
                        \itemindent    \listparindent
                        \rightmargin   \leftmargin
                        \parsep        \z@ \@plus\p@}%
                \item\relax}
               {\endlist}

\newenvironment{quote}
               {\list{}{\rightmargin\leftmargin}%
                \item\relax}
               {\endlist}
\if@compatibility

\newenvironment{titlepage}
    {%
      \if@twocolumn
        \@restonecoltrue\onecolumn
      \else
        \@restonecolfalse\newpage
      \fi
      \thispagestyle{empty}%
      \setcounter{page}\z@
    }%
    {\if@restonecol\twocolumn \else \newpage \fi
    }
\else

\newenvironment{titlepage}
    {%
      \if@twocolumn
        \@restonecoltrue\onecolumn
      \else
        \@restonecolfalse\newpage
      \fi
      \thispagestyle{empty}%
      \setcounter{page}\@ne
    }%
    {\if@restonecol\twocolumn \else \newpage \fi
     \if@twoside\else
        \setcounter{page}\@ne
     \fi
    }
\fi

\newcommand\appendix{\par
  \setcounter{section}{0}%
  \setcounter{subsection}{0}%
  \renewcommand\thesection{\@Alph\c@section}}

\setlength\arraycolsep{5\p@}
\setlength\tabcolsep{6\p@}
\setlength\arrayrulewidth{.4\p@}
\setlength\doublerulesep{2\p@}
\setlength\tabbingsep{\labelsep}
\skip\@mpfootins = \skip\footins
\setlength\fboxsep{3\p@}
\setlength\fboxrule{.4\p@}

\renewcommand \theequation {\@arabic\c@equation}

\newcounter{figure}

\renewcommand \thefigure {\@arabic\c@figure}
\def\fps@figure{tbp}
\def\ftype@figure{1}
\def\ext@figure{lof}
\def\fnum@figure{\figurename~\thefigure}

\newenvironment{figure}
               {\@float{figure}}
               {\end@float}

\newenvironment{figure*}
               {\@dblfloat{figure}}
               {\end@dblfloat}

\newcounter{table}

\renewcommand\thetable{\@arabic\c@table}
\def\fps@table{tbp}
\def\ftype@table{2}
\def\ext@table{lot}
\def\fnum@table{\tablename~\thetable}

\newenvironment{table}
               {\@float{table}}
               {\end@float}

\newenvironment{table*}
               {\@dblfloat{table}}
               {\end@dblfloat}

\newlength\abovecaptionskip
\newlength\belowcaptionskip
\setlength\abovecaptionskip{10\p@}
\setlength\belowcaptionskip{0\p@}

\long\def\@makecaption#1#2{%
  \vskip\abovecaptionskip
  \sbox\@tempboxa{\captionsize #1: #2 \normalsize}%
  \ifdim \wd\@tempboxa >\hsize
    \captionsize #1: #2 \normalsize \par
  \else
    \global \@minipagefalse
    \hb@xt@\hsize{\hfil\box\@tempboxa\hfil}%
  \fi
  \vskip\belowcaptionskip}

\DeclareOldFontCommand{\rm}{\normalfont\rmfamily}{\mathrm}
\DeclareOldFontCommand{\sf}{\normalfont\sffamily}{\mathsf}
\DeclareOldFontCommand{\tt}{\normalfont\ttfamily}{\mathtt}
\DeclareOldFontCommand{\bf}{\normalfont\bfseries}{\mathbf}
\DeclareOldFontCommand{\it}{\normalfont\itshape}{\mathit}
\DeclareOldFontCommand{\sl}{\normalfont\slshape}{\@nomath\sl}
\DeclareOldFontCommand{\sc}{\normalfont\scshape}{\@nomath\sc}
\DeclareRobustCommand*\cal{\@fontswitch\relax\mathcal}
\DeclareRobustCommand*\mit{\@fontswitch\relax\mathnormal}

\newcommand\@pnumwidth{1.55em}
\newcommand\@tocrmarg{2.55em}
\newcommand\@dotsep{4.5}
\setcounter{tocdepth}{3}

\newcommand\tableofcontents{%
    \section*{\contentsname
        \@mkboth{%
           \MakeUppercase\contentsname}{\MakeUppercase\contentsname}}%
    \@starttoc{toc}%
    }

\newcommand*\l@part[2]{%
  \ifnum \c@tocdepth >-2\relax
    \addpenalty\@secpenalty
    \addvspace{2.25em \@plus\p@}%
    \begingroup
      \parindent \z@ \rightskip \@pnumwidth
      \parfillskip -\@pnumwidth
      {\leavevmode
       \large \bfseries #1\hfil \hb@xt@\@pnumwidth{\hss #2}}\par
       \nobreak
       \if@compatibility
         \global\@nobreaktrue
         \everypar{\global\@nobreakfalse\everypar{}}%
      \fi
    \endgroup
  \fi}

\newcommand*\l@section[2]{%
  \ifnum \c@tocdepth >\z@
    \addpenalty\@secpenalty
    \addvspace{1.0em \@plus\p@}%
    \setlength\@tempdima{1.5em}%
    \begingroup
      \parindent \z@ \rightskip \@pnumwidth
      \parfillskip -\@pnumwidth
      \leavevmode \bfseries
      \advance\leftskip\@tempdima
      \hskip -\leftskip
      #1\nobreak\hfil \nobreak\hb@xt@\@pnumwidth{\hss #2}\par
    \endgroup
  \fi}

\newcommand*\l@subsection{\@dottedtocline{2}{1.5em}{2.3em}}
\newcommand*\l@subsubsection{\@dottedtocline{3}{3.8em}{3.2em}}
\newcommand*\l@paragraph{\@dottedtocline{4}{7.0em}{4.1em}}
\newcommand*\l@subparagraph{\@dottedtocline{5}{10em}{5em}}
\newcommand\listoffigures{%
    \section*{\listfigurename
      \@mkboth{\MakeUppercase\listfigurename}%
              {\MakeUppercase\listfigurename}}%
    \@starttoc{lof}%
    }
\newcommand*\l@figure{\@dottedtocline{1}{1.5em}{2.3em}}
\newcommand\listoftables{%
    \section*{\listtablename
      \@mkboth{%
          \MakeUppercase\listtablename}{\MakeUppercase\listtablename}}%
    \@starttoc{lot}%
    }

\let\l@table\l@figure
\newdimen\bibindent
\setlength\bibindent{1.5em}

\newenvironment{thebibliography}[1]
     {\section*{\refname
        \@mkboth{\MakeUppercase\refname}{\MakeUppercase\refname}}%
      \list{\@biblabel{\@arabic\c@enumiv}}%
           {
             %\itemsep      \z@
             \itemindent   -1.5em%
             \listparindent\itemindent
             %\rightmargin  \leftmargin
             %\advance\leftmargin 1.5em
             \settowidth\labelwidth{\@biblabel{#1}}%
            \leftmargin 1.5em 
            \advance\leftmargin\labelsep
            \@openbib@code
            \usecounter{enumiv}%
            \let\p@enumiv\@empty
            \renewcommand\theenumiv{\@arabic\c@enumiv}
             }%
      \sloppy
      \clubpenalty4000
      \@clubpenalty \clubpenalty
      \widowpenalty4000%
      \sfcode`\.\@m}
     {\def\@noitemerr
       {\@latex@warning{Empty `thebibliography' environment}}%
      \endlist}

\newcommand\newblock{\hskip .11em\@plus.33em\@minus.07em}
\let\@openbib@code\@empty

\newenvironment{theindex}
               {\if@twocolumn
                  \@restonecolfalse
                \else
                  \@restonecoltrue
                \fi
                \columnseprule \z@
                \columnsep 35\p@
                \twocolumn[\section*{\indexname}]%
                \@mkboth{\MakeUppercase\indexname}%
                        {\MakeUppercase\indexname}%
                \thispagestyle{plain}\parindent\z@
                \parskip\z@ \@plus .3\p@\relax
                \let\item\@idxitem}
              {\if@restonecol\onecolumn\else\clearpage\fi}
\newcommand\@idxitem{\par\hangindent 40\p@}
\newcommand\subitem{\@idxitem \hspace*{20\p@}}
\newcommand\subsubitem{\@idxitem \hspace*{30\p@}}
\newcommand\indexspace{\par \vskip 10\p@ \@plus5\p@ \@minus3\p@\relax}
\renewcommand\footnoterule{%
  \kern-3\p@
  \hrule\@width.4\columnwidth
  \kern2.6\p@}
\newcommand\@makefntext[1]{%
    \parindent 1em%
    \noindent
    \hb@xt@1.8em{\hss\@makefnmark}#1}
\newcommand\contentsname{Contents}
\newcommand\listfigurename{List of Figures}
\newcommand\listtablename{List of Tables}
\newcommand\refname{References}
\newcommand\indexname{Index}
\newcommand\figurename{Figure}
\newcommand\tablename{Table}
\newcommand\partname{Part}
\newcommand\appendixname{Appendix}
\newcommand\abstractname{Abstract}
\def\today{\ifcase\month\or
  January\or February\or March\or April\or May\or June\or
  July\or August\or September\or October\or November\or December\fi
  \space\number\day, \number\year}
\setlength\columnsep{10\p@}
\setlength\columnseprule{0\p@}
\pagestyle{empty}
%\pagestyle{headings}
\pagenumbering{arabic}
\if@twoside
\else
  \raggedbottom
\fi
\if@twocolumn
  \twocolumn
  \sloppy
  \flushbottom
\else
  \onecolumn
\fi
\setlength\textheight     {9.25in}%
\setlength\textwidth      {7in}
\setlength\oddsidemargin  {-0.25in} %start from 1 inch
\setlength\evensidemargin {-0.25in} %start from 1 inch
\setlength\topmargin      {0mm}     %start from 1inch 
\setlength\headsep        {0mm}
\setlength\headheight     {0mm}
%\setlength\headsep        {10mm}
%\setlength\headheight     {4.2mm}
\setlength\columnsep      {0.25in}
\marginparwidth 107pt
\marginparsep 11pt
\arraycolsep 5pt 
\tabcolsep 6pt 
\arrayrulewidth .4pt 
\doublerulesep 2pt
\tabbingsep \labelsep



%%


\renewcommand\footnotesize{\@setsize\footnotesize{9pt}\viiipt\@viiipt
\abovedisplayskip 6pt plus 2pt minus 4pt\belowdisplayskip \abovedisplayskip
\abovedisplayshortskip \z@ plus 1pt\belowdisplayshortskip 3pt plus 1pt minus
2pt
\def\@listi{\leftmargin\leftmargini \topsep 3pt plus 1pt minus 1pt\parsep 2pt
plus 1pt minus 1pt
\itemsep \parsep}}

\renewcommand\normalsize{\@setsize\normalsize{12pt}\xpt\@xpt
\abovedisplayskip 10pt plus2pt minus5pt\belowdisplayskip \abovedisplayskip
\abovedisplayshortskip \z@ plus3pt\belowdisplayshortskip 6pt plus3pt
minus3pt\let\@listi\@listI}

\renewcommand\small{\@setsize\small{11pt}\ixpt\@ixpt
\abovedisplayskip 8.5pt plus 3pt minus 4pt\belowdisplayskip \abovedisplayskip
\abovedisplayshortskip \z@ plus2pt\belowdisplayshortskip 4pt plus2pt minus 2pt
\def\@listi{\leftmargin\leftmargini \topsep 4pt plus 2pt minus 2pt\parsep 2pt
plus 1pt minus 1pt
\itemsep \parsep}}

\renewcommand\scriptsize{\@setsize\scriptsize{8pt}\viipt\@viipt}
\newcommand\captionsize{\@setsize\captionsize{11pt}\ixpt\@ixpt}
\renewcommand\tiny{\@setsize\tiny{6pt}\vpt\@vpt}
\renewcommand\large{\@setsize\large{14pt}\xiipt\@xiipt}
\renewcommand\Large{\@setsize\Large{18pt}\xivpt\@xivpt}
%\renewcommand\LARGE{\@setsize\LARGE{22pt}\xviipt\@xviipt}
\renewcommand\LARGE{\fontseries{b}\fontsize{22pt}{28pt}\selectfont}
\renewcommand\huge{\@setsize\huge{25pt}\xxpt\@xxpt}
\renewcommand\Huge{\@setsize\Huge{30pt}\xxvpt\@xxvpt}

\renewcommand\labelenumi{\theenumi.}
\renewcommand\theenumi{\arabic{enumi}}
\renewcommand\labelenumii{(\theenumii)}
\renewcommand\theenumii{\alph{enumii}}
\renewcommand\p@enumii{\theenumi}
\renewcommand\labelenumiii{\theenumiii.}
\renewcommand\theenumiii{\roman{enumiii}}
\renewcommand\p@enumiii{\theenumi(\theenumii)}
\renewcommand\labelenumiv{\theenumiv.}
\renewcommand\theenumiv{\Alph{enumiv}}
\renewcommand\p@enumiv{\p@enumiii\theenumiii}


\endinput

%% End of file `sab.cls'.
