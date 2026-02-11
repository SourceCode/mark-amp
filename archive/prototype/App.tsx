import React, { useState, useEffect } from 'react';
import { THEMES, INITIAL_FILES } from './constants';
import { Theme, FileNode, ViewMode } from './types';
import Sidebar from './components/Sidebar';
import Preview from './components/Preview';
import ThemeGallery from './components/ThemeGallery';
import { 
  Maximize2, 
  Minimize2, 
  X, 
  Settings, 
  Code, 
  Eye, 
  Columns,
  Menu,
  Save,
  Palette
} from 'lucide-react';

// Custom CSS implementation for editor to match theme
const Editor: React.FC<{ content: string; onChange: (val: string) => void }> = ({ content, onChange }) => {
  return (
    <textarea
      value={content}
      onChange={(e) => onChange(e.target.value)}
      className="w-full h-full bg-win-input text-win-text p-6 font-mono text-sm resize-none focus:outline-none custom-scrollbar border-none placeholder-win-muted/50"
      placeholder="Start typing..."
      spellCheck={false}
    />
  );
};

const App: React.FC = () => {
  const [customThemes, setCustomThemes] = useState<Theme[]>([]);
  const [theme, setTheme] = useState<Theme>(THEMES[0]);
  const [files, setFiles] = useState<FileNode[]>(INITIAL_FILES);
  const [activeFileId, setActiveFileId] = useState<string>('readme');
  const [activeContent, setActiveContent] = useState<string>('');
  const [viewMode, setViewMode] = useState<ViewMode>(ViewMode.SPLIT);
  const [sidebarVisible, setSidebarVisible] = useState(true);
  const [galleryOpen, setGalleryOpen] = useState(false);

  // Initialize content
  useEffect(() => {
    const findFile = (nodes: FileNode[], id: string): FileNode | null => {
      for (const node of nodes) {
        if (node.id === id) return node;
        if (node.children) {
          const found = findFile(node.children, id);
          if (found) return found;
        }
      }
      return null;
    };

    const file = findFile(files, activeFileId);
    if (file && file.content !== undefined) {
      setActiveContent(file.content);
    }
  }, [activeFileId, files]);

  // Apply theme variables to root style
  const themeStyles = Object.entries(theme.colors).reduce((acc, [key, value]) => {
    acc[key as any] = value;
    return acc;
  }, {} as React.CSSProperties);

  const handleFileSelect = (file: FileNode) => {
    setActiveFileId(file.id);
  };

  const handleContentChange = (newContent: string) => {
    setActiveContent(newContent);
    // In a real app, we would update the file tree state here
  };

  const handleImportTheme = (newTheme: Theme) => {
    // Avoid duplicates by ID
    setCustomThemes(prev => {
      if (prev.some(t => t.id === newTheme.id)) return prev;
      return [...prev, newTheme];
    });
    setTheme(newTheme);
  };

  const allThemes = [...THEMES, ...customThemes];

  return (
    <div 
      className="h-screen w-screen flex flex-col font-sans text-win-text bg-win-bg overflow-hidden transition-colors duration-300"
      style={themeStyles}
    >
      <ThemeGallery 
        isOpen={galleryOpen}
        onClose={() => setGalleryOpen(false)}
        themes={allThemes}
        currentTheme={theme}
        onApplyTheme={setTheme}
        onImportTheme={handleImportTheme}
      />

      {/* --- WINDOW CHROME (Top Bar) --- */}
      <div className="h-10 bg-win-header flex items-center justify-between px-3 select-none border-b-2 border-b-win-borderDark border-t border-t-win-borderLight draggable">
        {/* Left: App Identity */}
        <div className="flex items-center space-x-3">
           <div className="w-4 h-4 rounded-full bg-gradient-to-br from-win-accent to-purple-600 shadow-[0_0_10px_var(--accent-primary)]" />
           <span className="font-bold tracking-wider text-sm text-win-accent uppercase drop-shadow-sm">MarkAmp</span>
           <span className="text-xs text-win-muted hidden sm:inline-block">v1.0.0</span>
        </div>

        {/* Center: File Name (Simulated) */}
        <div className="text-xs font-mono text-win-muted bg-black/20 px-3 py-1 rounded border border-win-borderLight/20 hidden md:block">
           {activeFileId}.md
        </div>

        {/* Right: Window Controls (Simulated) */}
        <div className="flex items-center space-x-2">
          <button 
            className="p-1.5 hover:bg-white/10 rounded text-win-muted hover:text-win-text transition-colors"
            onClick={() => setSidebarVisible(!sidebarVisible)}
            title="Toggle Sidebar"
          >
            <Menu size={14} />
          </button>
           <div className="h-4 w-[1px] bg-win-borderLight/30 mx-1" />
          <button className="p-1.5 hover:bg-white/10 rounded text-win-muted hover:text-win-text"><Minimize2 size={12} /></button>
          <button className="p-1.5 hover:bg-white/10 rounded text-win-muted hover:text-win-text"><Maximize2 size={12} /></button>
          <button className="p-1.5 hover:bg-red-500/80 rounded text-win-muted hover:text-white"><X size={12} /></button>
        </div>
      </div>

      {/* --- MAIN LAYOUT --- */}
      <div className="flex-1 flex overflow-hidden">
        
        {/* Sidebar */}
        {sidebarVisible && (
          <div className="w-64 flex-shrink-0 z-10 shadow-xl">
            <Sidebar 
              files={files} 
              activeFileId={activeFileId} 
              onFileSelect={handleFileSelect} 
            />
          </div>
        )}

        {/* Editor Area */}
        <div className="flex-1 flex flex-col min-w-0 bg-win-bg relative">
          
          {/* Toolbar */}
          <div className="h-10 bg-win-panel border-b border-win-borderDark flex items-center justify-between px-4">
            <div className="flex items-center space-x-1">
               <button 
                 onClick={() => setViewMode(ViewMode.EDITOR)}
                 className={`p-1.5 rounded flex items-center space-x-1.5 text-xs font-semibold transition-all ${viewMode === ViewMode.EDITOR ? 'bg-win-accent/20 text-win-accent shadow-sm' : 'text-win-muted hover:text-win-text'}`}
               >
                 <Code size={14} /> <span>SRC</span>
               </button>
               <button 
                 onClick={() => setViewMode(ViewMode.SPLIT)}
                 className={`p-1.5 rounded flex items-center space-x-1.5 text-xs font-semibold transition-all ${viewMode === ViewMode.SPLIT ? 'bg-win-accent/20 text-win-accent shadow-sm' : 'text-win-muted hover:text-win-text'}`}
               >
                 <Columns size={14} /> <span>SPLIT</span>
               </button>
               <button 
                 onClick={() => setViewMode(ViewMode.PREVIEW)}
                 className={`p-1.5 rounded flex items-center space-x-1.5 text-xs font-semibold transition-all ${viewMode === ViewMode.PREVIEW ? 'bg-win-accent/20 text-win-accent shadow-sm' : 'text-win-muted hover:text-win-text'}`}
               >
                 <Eye size={14} /> <span>VIEW</span>
               </button>
            </div>

            <div className="flex items-center space-x-3">
              <button className="text-win-muted hover:text-win-accent transition-colors" title="Save">
                <Save size={16} />
              </button>
              
              <div className="relative">
                <button 
                  onClick={() => setGalleryOpen(true)}
                  className="text-win-muted hover:text-win-accent transition-colors flex items-center space-x-1" 
                  title="Theme Gallery"
                >
                  <Palette size={16} />
                  <span className="text-xs font-semibold hidden md:inline">THEMES</span>
                </button>
              </div>

              <button className="text-win-muted hover:text-win-accent transition-colors">
                <Settings size={16} />
              </button>
            </div>
          </div>

          {/* Content Wrapper */}
          <div className="flex-1 flex overflow-hidden">
            {/* Source Editor Pane */}
            {(viewMode === ViewMode.EDITOR || viewMode === ViewMode.SPLIT) && (
              <div className={`flex-1 flex flex-col relative ${viewMode === ViewMode.SPLIT ? 'border-r border-win-borderDark' : ''}`}>
                 <Editor content={activeContent} onChange={handleContentChange} />
                 {/* Bevel Effects */}
                 <div className="absolute inset-0 pointer-events-none border-inset border-2 border-t-black/20 border-l-black/20 border-b-white/5 border-r-white/5" />
              </div>
            )}

            {/* Preview Pane */}
            {(viewMode === ViewMode.PREVIEW || viewMode === ViewMode.SPLIT) && (
              <div className="flex-1 bg-win-bg overflow-y-auto custom-scrollbar relative">
                <div className="min-h-full">
                  <Preview content={activeContent} />
                </div>
                 {/* Bevel Effects */}
                 <div className="absolute inset-0 pointer-events-none border-inset border-2 border-t-black/20 border-l-black/20 border-b-white/5 border-r-white/5" />
              </div>
            )}
          </div>

          {/* Status Bar */}
          <div className="h-6 bg-win-panel border-t border-t-win-borderLight flex items-center justify-between px-3 text-[10px] font-mono text-win-muted uppercase select-none">
             <div className="flex items-center space-x-4">
               <span>Ready</span>
               <span>Ln 12, Col 42</span>
               <span>UTF-8</span>
             </div>
             <div className="flex items-center space-x-4">
               <span className="text-win-accent">Mermaid: Active</span>
               <span>{theme.name}</span>
             </div>
          </div>

        </div>
      </div>
    </div>
  );
};

export default App;
