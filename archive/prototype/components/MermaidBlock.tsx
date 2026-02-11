import React, { useEffect, useRef, useState } from 'react';
import mermaid from 'mermaid';

interface MermaidBlockProps {
  chart: string;
}

const MermaidBlock: React.FC<MermaidBlockProps> = ({ chart }) => {
  const containerRef = useRef<HTMLDivElement>(null);
  const [svgContent, setSvgContent] = useState<string>('');
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    mermaid.initialize({
      startOnLoad: false,
      theme: 'dark',
      securityLevel: 'loose',
      fontFamily: 'JetBrains Mono',
    });
  }, []);

  useEffect(() => {
    const renderChart = async () => {
      if (!containerRef.current || !chart) return;
      
      const id = `mermaid-${Math.random().toString(36).substr(2, 9)}`;
      
      try {
        setError(null);
        // mermaid.render returns an object with 'svg' property in newer versions
        const { svg } = await mermaid.render(id, chart);
        setSvgContent(svg);
      } catch (err) {
        console.error('Mermaid render error:', err);
        setError('Syntax Error: Invalid Mermaid definition');
      }
    };

    renderChart();
  }, [chart]);

  if (error) {
    return (
      <div className="p-4 border border-red-500/50 bg-red-900/20 text-red-400 font-mono text-sm rounded">
        {error}
      </div>
    );
  }

  return (
    <div 
      className="mermaid-container my-4 flex justify-center bg-win-panel/50 p-4 rounded border border-win-borderLight"
      ref={containerRef}
      dangerouslySetInnerHTML={{ __html: svgContent }}
    />
  );
};

export default MermaidBlock;
