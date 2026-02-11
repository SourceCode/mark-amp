import React, { useState } from 'react';
import { FileNode } from '../types';
import { ChevronRight, ChevronDown, FileText, Folder, FolderOpen } from 'lucide-react';

interface SidebarProps {
  files: FileNode[];
  activeFileId: string | null;
  onFileSelect: (file: FileNode) => void;
}

const FileTreeItem: React.FC<{
  node: FileNode;
  depth: number;
  activeFileId: string | null;
  onFileSelect: (file: FileNode) => void;
}> = ({ node, depth, activeFileId, onFileSelect }) => {
  const [isOpen, setIsOpen] = useState(node.isOpen || false);

  const handleToggle = (e: React.MouseEvent) => {
    e.stopPropagation();
    if (node.type === 'folder') {
      setIsOpen(!isOpen);
    } else {
      onFileSelect(node);
    }
  };

  const isSelected = node.id === activeFileId;

  return (
    <div>
      <div
        className={`
          flex items-center py-1 px-2 cursor-pointer select-none
          transition-colors duration-100 font-mono text-sm
          ${isSelected ? 'bg-win-accent/20 text-win-accent' : 'text-win-muted hover:text-win-text hover:bg-win-accent/10'}
        `}
        style={{ paddingLeft: `${depth * 16 + 8}px` }}
        onClick={handleToggle}
      >
        <span className="mr-2 opacity-70">
          {node.type === 'folder' ? (
            isOpen ? <FolderOpen size={14} /> : <Folder size={14} />
          ) : (
            <FileText size={14} />
          )}
        </span>
        <span className="truncate">{node.name}</span>
        {node.type === 'folder' && (
          <span className="ml-auto opacity-50">
            {isOpen ? <ChevronDown size={12} /> : <ChevronRight size={12} />}
          </span>
        )}
      </div>
      {node.type === 'folder' && isOpen && node.children && (
        <div>
          {node.children.map((child) => (
            <FileTreeItem
              key={child.id}
              node={child}
              depth={depth + 1}
              activeFileId={activeFileId}
              onFileSelect={onFileSelect}
            />
          ))}
        </div>
      )}
    </div>
  );
};

const Sidebar: React.FC<SidebarProps> = ({ files, activeFileId, onFileSelect }) => {
  return (
    <div className="h-full flex flex-col bg-win-panel border-r-2 border-r-win-borderDark border-l border-l-win-borderLight">
      {/* Sidebar Header */}
      <div className="px-4 py-3 border-b border-win-borderDark bg-win-header/50">
        <h2 className="text-win-accent text-xs font-bold uppercase tracking-widest font-sans">
          Explorer
        </h2>
      </div>

      {/* Files List */}
      <div className="flex-1 overflow-y-auto py-2 custom-scrollbar">
        {files.map((node) => (
          <FileTreeItem
            key={node.id}
            node={node}
            depth={0}
            activeFileId={activeFileId}
            onFileSelect={onFileSelect}
          />
        ))}
      </div>

      {/* Sidebar Footer Stats */}
      <div className="p-3 border-t border-win-borderLight bg-win-bg/50 text-[10px] text-win-muted font-mono">
        <div className="flex justify-between mb-1">
          <span>FILES</span>
          <span>{files.length}</span>
        </div>
        <div className="flex justify-between">
          <span>SIZE</span>
          <span>1.2MB</span>
        </div>
      </div>
    </div>
  );
};

export default Sidebar;
